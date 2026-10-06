#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    throw std::invalid_argument("too few args");
  }
  size_t threads, tries;
  size_t seed = 0;
  try {
    threads = std::stoull(argv[1]);
    tries = std::stoull(argv[2]);

    if (argc >= 4) {
      seed = std::stoull(argv[3]);
    }
  } catch (...) {
    throw std::invalid_argument("bad input params");
  }

  size_t triesPerTest = tries / threads;
  levkin::CircleVec dataVec;
  
  for (size_t i = 0; i < threads; ++i) {
      dataVec.emplace_back();
      levkin::Circle& circle = dataVec[i];
      
      if (!(std::cin >> circle)) {
          throw std::invalid_argument("bad input file");
      }
  }
  
  
}
