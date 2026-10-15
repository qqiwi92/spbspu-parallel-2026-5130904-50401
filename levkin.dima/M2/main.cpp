#include <iostream>
#include "io.hpp"
int main()
{
  levkin::IO io{};
  
  std::cout << "levkin.dima\n" << sizeof(io);
}
