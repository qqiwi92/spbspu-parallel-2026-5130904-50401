#include "utils.hpp"
#include <cstddef>
#include <future>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    throw std::invalid_argument("too few args");
  }
  size_t threads, tries;
  size_t seed = 0;
  try
  {
    threads = std::stoull(argv[1]);
    tries = std::stoull(argv[2]);

    if (argc >= 4)
    {
      seed = std::stoull(argv[3]);
    }
  }
  catch (...)
  {
    throw std::invalid_argument("bad input params");
  }

  size_t triesPerTest = tries / threads;

  levkin::CircleVec data = levkin::getData(std::cin);

  std::vector< std::future< size_t > > futures;
  futures.reserve(threads);
  for (size_t i = 0; i < threads; ++i)
  {
    futures.emplace_back(std::async(levkin::runTests, triesPerTest, seed));
  }

  size_t totalGot = 0;
  for (auto &ft : futures)
  {
    totalGot += ft.get();
  }

  std::cout << static_cast< double >(totalGot) / tries << '\n';
}
