#include <cstddef>
#include <future>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "utils.hpp"

int main(int argc, char *argv[])
{
  if (argc < 3 || argc > 4) {
    std::cerr << "too few args\n";
    return 1;
  }

  long long threads_input = 0;
  long long tries_input = 0;
  long long seed_input = 0;

  try {
    size_t pos = 0;
    threads_input = std::stoll(argv[1], &pos);
    if (argv[1][pos] != '\0') {
      std::cerr << "bad args\n";
      return 1;
    }

    pos = 0;
    tries_input = std::stoll(argv[2], &pos);
    if (argv[2][pos] != '\0') {
      std::cerr << "bad args\n";
      return 1;
    }

    if (argc == 4) {
      pos = 0;
      seed_input = std::stoll(argv[3], &pos);
      if (argv[3][pos] != '\0') {
        std::cerr << "bad args\n";
        return 1;
      }
    }
  } catch (...) {
    std::cerr << "bad params\n";
    return 1;
  }

  if (tries_input <= 0 || seed_input < 0 || threads_input < 0) {
    std::cerr << "bad params\n";
    return 1;
  }

  const size_t tries = static_cast< size_t >(tries_input);
  const size_t seed = static_cast< size_t >(seed_input);
  const size_t requested_threads = static_cast< size_t >(threads_input);

  size_t threads = (requested_threads == 0) ? 1 : requested_threads;
  threads = std::min(threads, tries);

  levkin::CirclesWithRanges cv;
  try {
    cv = levkin::getData(std::cin);
  } catch (const std::exception &e) {
    std::cerr << "bad input: " << e.what() << '\n';
    return 2;
  }

  if (cv.circles.empty()) {
    std::cout << "0 0\n";
    return 0;
  }

  const size_t base_tries = tries / threads;
  const size_t remainder = tries % threads;

  std::vector< std::future< levkin::AnyAllResult > > futures;
  futures.reserve(threads);

  for (size_t i = 0; i < threads; ++i) {
    const size_t thread_tries = base_tries + (i < remainder ? 1 : 0);
    if (thread_tries > 0) {
      futures.emplace_back(std::async(std::launch::async, levkin::runTests, thread_tries, seed + i, std::cref(cv)));
    }
  }

  levkin::AnyAllResult hit{};
  for (auto &ft : futures) {
    hit += ft.get();
  }

  const double box_area = static_cast< double >(cv.rangeArea());
  const double res_any = (static_cast< double >(hit.any) / static_cast< double >(tries)) * box_area;
  const double res_all = (static_cast< double >(hit.all) / static_cast< double >(tries)) * box_area;

  std::cout.precision(std::numeric_limits< double >::max_digits10);
  std::cout << res_any << " " << res_all << "\n";

  return 0;
}
