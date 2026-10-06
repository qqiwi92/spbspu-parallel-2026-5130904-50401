#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include <istream>
#include <ostream>
#include <vector>
namespace levkin
{
  struct Circle
  {
    int x, y, radius;
  };

  using CircleVec = std::vector< Circle >;
  std::istream &operator>>(std::istream &is, Circle &c);

  CircleVec getData(std::istream& is); 
  bool gotIn(levkin::Circle c, int x, int y);
  bool gotIn(levkin::CircleVec cv, int x, int y);
  size_t runTests(size_t howManyTests, size_t seed);

}
#endif
