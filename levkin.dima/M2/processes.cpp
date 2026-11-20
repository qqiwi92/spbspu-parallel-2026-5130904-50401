#include "processes.hpp"

#include "calculations.hpp"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <limits>
#include <poll.h>
#include <stdexcept>
#include <unistd.h>
#include <sys/wait.h>

namespace levkin {
  namespace detail {
    struct Task {
      size_t calc_id = 0;
      size_t count = 0;
      size_t threads = 0;
      size_t tries = 0;
    };

    struct Result {
      size_t calc_id = 0;
      double milliseconds = 0;
      double area = 0;
    };

    size_t recvAll(int &err, int rd, char *buffer, size_t size)
    {
      err = 0;
      size_t received = 0;
      while (received < size) {
        const size_t chunk = std::min(size - received, static_cast< size_t >(std::numeric_limits< ssize_t >::max()));
        const ssize_t count = read(rd, buffer + received, chunk);
        if (count == -1) {
          if (errno == EINTR) {
            continue;
          }
          err = errno;
          break;
        }
        if (count == 0) {
          break;
        }
        received += static_cast< size_t >(count);
      }
      return received;
    }

    size_t sendAll(int &err, int wr, const char *buffer, size_t size)
    {
      err = 0;
      size_t sent = 0;
      while (sent < size) {
        const size_t chunk = std::min(size - sent, static_cast< size_t >(std::numeric_limits< ssize_t >::max()));
        const ssize_t count = write(wr, buffer + sent, chunk);
        if (count == -1) {
          if (errno == EINTR) {
            continue;
          }
          err = errno;
          break;
        }
        if (count == 0) {
          err = EIO;
          break;
        }
        sent += static_cast< size_t >(count);
      }
      return sent;
    }

    void calculationLoop(int task_fd, int result_fd, size_t seed)
    {
      int exit_code = 0;
      try {
        struct sigaction action{};
        action.sa_handler = SIG_IGN;
        if (sigemptyset(&action.sa_mask) == -1 || sigaction(SIGPIPE, &action, nullptr) == -1) {
          close(task_fd);
          close(result_fd);
          _exit(1);
        }

        std::vector< ellipse_t > ellipses;
        while (true) {
          Task task{};
          int err = 0;
          const size_t received = recvAll(err, task_fd, reinterpret_cast< char * >(&task), sizeof(task));
          if (received == 0 && err == 0) {
            break;
          }
          if (err != 0 || received != sizeof(task) || task.count == 0 || task.threads == 0 || task.tries == 0) {
            exit_code = 1;
            break;
          }
          if (task.count > ellipses.max_size()
              || task.count > std::numeric_limits< size_t >::max() / sizeof(ellipse_t)) {
            exit_code = 1;
            break;
          }

          ellipses.resize(task.count);
          const size_t bytes = task.count * sizeof(ellipse_t);
          const size_t body_received = recvAll(err, task_fd, reinterpret_cast< char * >(ellipses.data()), bytes);
          if (err != 0 || body_received != bytes) {
            exit_code = 1;
            break;
          }

          const auto start = std::chrono::steady_clock::now();
          const double area = areaOf(ellipses, task.threads, task.tries, seed);
          const auto finish = std::chrono::steady_clock::now();
          const std::chrono::duration< double > elapsed = finish - start;

          const Result result{task.calc_id, elapsed.count(), area};
          const size_t sent = sendAll(err, result_fd, reinterpret_cast< const char * >(&result), sizeof(result));
          if (err != 0 || sent != sizeof(result)) {
            exit_code = 1;
            break;
          }
        }
      } catch (...) {
        exit_code = 1;
      }

      close(task_fd);
      close(result_fd);
      _exit(exit_code);
    }
  }
}

void levkin::spawnProcess(App &app, const std::string &name, size_t seed)
{
  int task_pipe[2]{-1, -1};
  int result_pipe[2]{-1, -1};

  if (pipe(task_pipe) == -1) {
    throw std::runtime_error("cannot create task pipe");
  }
  if (pipe(result_pipe) == -1) {
    close(task_pipe[0]);
    close(task_pipe[1]);
    throw std::runtime_error("cannot create result pipe");
  }

  const pid_t pid = fork();
  if (pid == -1) {
    close(task_pipe[0]);
    close(task_pipe[1]);
    close(result_pipe[0]);
    close(result_pipe[1]);
    throw std::runtime_error("cannot fork process");
  }

  if (pid == 0) {
    close(task_pipe[1]);
    close(result_pipe[0]);
    detail::calculationLoop(task_pipe[0], result_pipe[1], seed);
  }

  close(task_pipe[0]);
  close(result_pipe[1]);

  const int flags = fcntl(result_pipe[0], F_GETFL, 0);
  if (flags == -1 || fcntl(result_pipe[0], F_SETFL, flags | O_NONBLOCK) == -1) {
    close(task_pipe[1]);
    close(result_pipe[0]);
    throw std::runtime_error("cannot set nonblocking mode");
  }

  process_t process{};
  process.pid = pid;
  process.seed = seed;
  process.task_fd = task_pipe[1];
  process.result_fd = result_pipe[0];
  process.state = ProcessState::Active;

  app.processes.insert({name, std::move(process)});
}

void levkin::startNextCalculation(App &app, process_t &process)
{
  if (process.state != ProcessState::Active || !process.current_calc_name.empty() || process.pending_calcs.empty()) {
    return;
  }

  const std::string calc_name = process.pending_calcs.front();
  process.pending_calcs.pop_front();
  calc_t &calc = app.calcs.at(calc_name);

  detail::Task task{calc.id, calc.ellipses.size(), calc.threads, calc.tries};
  int err = 0;
  const size_t sent = detail::sendAll(err, process.task_fd, reinterpret_cast< const char * >(&task), sizeof(task));
  if (err != 0 || sent != sizeof(task)) {
    calc.state = TaskState::Failed;
    throw std::runtime_error("cannot send task header to process");
  }

  const size_t bytes = calc.ellipses.size() * sizeof(ellipse_t);
  const size_t body_sent = detail::sendAll(
      err, process.task_fd, reinterpret_cast< const char * >(calc.ellipses.data()), bytes);
  if (err != 0 || body_sent != bytes) {
    calc.state = TaskState::Failed;
    throw std::runtime_error("cannot send task payload to process");
  }

  calc.state = TaskState::Running;
  process.current_calc_name = calc_name;
}

void levkin::receiveAvailableResult(App &app, process_t &process)
{
  if (process.result_fd < 0) {
    return;
  }

  char chunk[256]{};
  while (true) {
    const ssize_t count = read(process.result_fd, chunk, sizeof(chunk));
    if (count == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        break;
      }
      if (errno == EINTR) {
        continue;
      }
      break;
    }
    if (count == 0) {
      break;
    }
    process.buffer.insert(process.buffer.end(), chunk, chunk + count);
  }

  while (process.buffer.size() >= sizeof(detail::Result)) {
    detail::Result result{};
    std::memcpy(&result, process.buffer.data(), sizeof(result));
    process.buffer.erase(process.buffer.begin(), process.buffer.begin() + sizeof(detail::Result));

    if (!process.current_calc_name.empty()) {
      calc_t &calc = app.calcs.at(process.current_calc_name);
      if (calc.id == result.calc_id) {
        calc.milliseconds = result.milliseconds;
        calc.result = result.area;
        calc.state = TaskState::Completed;
        process.current_calc_name.clear();
        startNextCalculation(app, process);
      }
    }
  }
}

void levkin::checkProcesses(App &app)
{
  for (auto &entry : app.processes) {
    process_t &process = entry.second;
    receiveAvailableResult(app, process);
  }
}

void levkin::waitCalculation(App &app, calc_t &calc)
{
  checkProcesses(app);
  while (calc.state == TaskState::Pending || calc.state == TaskState::Running) {
    const process_t &process = app.processes.at(calc.process_name);
    if (process.result_fd < 0) {
      throw std::runtime_error("result pipe is closed");
    }
    pollfd descriptor{process.result_fd, POLLIN, 0};
    int result = 0;
    do {
      result = poll(&descriptor, 1, -1);
    } while (result == -1 && errno == EINTR);
    if (result == -1) {
      throw std::runtime_error("poll failed");
    }
    checkProcesses(app);
  }
}

void levkin::waitCalculation(App &app, calc_t &calc, long long timeout_ms)
{
  checkProcesses(app);
  const auto start = std::chrono::steady_clock::now();
  while (calc.state == TaskState::Pending || calc.state == TaskState::Running) {
    const auto elapsed
        = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::steady_clock::now() - start);
    if (elapsed.count() >= timeout_ms) {
      break;
    }

    const process_t &process = app.processes.at(calc.process_name);
    if (process.result_fd < 0) {
      throw std::runtime_error("result pipe is closed");
    }
    const long long remaining = timeout_ms - elapsed.count();
    const int poll_timeout
        = static_cast< int >(std::min(remaining, static_cast< long long >(std::numeric_limits< int >::max())));
    pollfd descriptor{process.result_fd, POLLIN, 0};
    const int result = poll(&descriptor, 1, poll_timeout);
    if (result == -1) {
      if (errno == EINTR) {
        continue;
      }
      throw std::runtime_error("poll failed");
    }
    checkProcesses(app);
  }
}

void levkin::killProcess(App &app, process_t &process)
{
  process.state = ProcessState::Stopping;
  for (const std::string &calc_name : process.pending_calcs) {
    calc_t &calc = app.calcs.at(calc_name);
    if (calc.state == TaskState::Pending) {
      calc.state = TaskState::Cancelled;
    }
  }
  process.pending_calcs.clear();
  if (process.task_fd >= 0) {
    close(process.task_fd);
    process.task_fd = -1;
  }

  pid_t result = 0;
  do {
    result = waitpid(process.pid, nullptr, 0);
  } while (result == -1 && errno == EINTR);
  if (result == -1 && errno != ECHILD) {
    throw std::runtime_error("waitpid failed");
  }
  if (process.result_fd >= 0) {
    try {
      receiveAvailableResult(app, process);
    } catch (...) {
    }
  }
  if (!process.current_calc_name.empty()) {
    calc_t &calc = app.calcs.at(process.current_calc_name);
    if (calc.state == TaskState::Running) {
      calc.state = TaskState::Failed;
    }
  }
  if (process.result_fd >= 0) {
    close(process.result_fd);
  }
  process.result_fd = -1;
  process.buffer.clear();
  process.current_calc_name.clear();
  process.state = ProcessState::Stopped;
}

void levkin::shutdownProcesses(App &app)
{
  for (auto &entry : app.processes) {
    process_t &process = entry.second;
    if (process.state == ProcessState::Stopped) {
      continue;
    }
    process.state = ProcessState::Stopping;
    if (process.task_fd >= 0) {
      close(process.task_fd);
      process.task_fd = -1;
    }
    for (const std::string &calc_name : process.pending_calcs) {
      calc_t &calc = app.calcs.at(calc_name);
      if (calc.state == TaskState::Pending) {
        calc.state = TaskState::Failed;
      }
    }
    process.pending_calcs.clear();
  }

  int wait_error = 0;
  for (auto &entry : app.processes) {
    process_t &process = entry.second;
    if (process.state == ProcessState::Stopped) {
      continue;
    }
    pid_t result = 0;
    do {
      result = waitpid(process.pid, nullptr, 0);
    } while (result == -1 && errno == EINTR);
    if (result == -1 && errno != ECHILD) {
      wait_error = errno;
    }
    if (process.result_fd >= 0) {
      try {
        receiveAvailableResult(app, process);
      } catch (...) {
      }
    }
    if (!process.current_calc_name.empty()) {
      calc_t &calc = app.calcs.at(process.current_calc_name);
      if (calc.state == TaskState::Running) {
        calc.state = TaskState::Failed;
      }
    }
    if (process.result_fd >= 0) {
      close(process.result_fd);
    }
    process.result_fd = -1;
    process.buffer.clear();
    process.current_calc_name.clear();
    process.state = ProcessState::Stopped;
  }
  if (wait_error != 0) {
    throw std::runtime_error(std::strerror(wait_error));
  }
}
