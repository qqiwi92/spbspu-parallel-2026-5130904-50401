#include "commands.hpp"
#include "geometry.hpp"

#include <csignal>
#include <iostream>
#include <unordered_map>

int main()
{
  struct sigaction action{};
  action.sa_handler = SIG_IGN;
  if (sigemptyset(&action.sa_mask) == -1 || sigaction(SIGPIPE, &action, nullptr) == -1) {
    std::cerr << "cannot configure SIGPIPE\n";
    return 1;
  }

  levkin::ellipses_t ellipses;
  levkin::sets_t sets;
  levkin::processes_t processes;
  levkin::calcs_t calcs;

  levkin::App app{ellipses, sets, processes, calcs};

  std::unordered_map< std::string, levkin::cmd_t > cmds;
  cmds["circle"] = levkin::cmdCircle;
  cmds["ellipse"] = levkin::cmdEllipse;
  cmds["rotate"] = levkin::cmdRotate;
  cmds["set"] = levkin::cmdSet;
  cmds["show"] = levkin::cmdShow;
  cmds["showset"] = levkin::cmdShowSet;
  cmds["frame"] = levkin::cmdFrame;
  cmds["frameset"] = levkin::cmdFrameSet;
  cmds["spawn"] = levkin::cmdSpawn;
  cmds["area-on"] = levkin::cmdAreaOn;
  cmds["status"] = levkin::cmdStatus;
  cmds["wait"] = levkin::cmdWait;
  cmds["timeout"] = levkin::cmdTimeout;
  cmds["kill"] = levkin::cmdKill;

  auto cmd_io = levkin::CommandIO{std::cout, cmds, app};
  levkin::runCommands(std::cin, cmd_io);
  levkin::shutdownProcesses(app);
}
