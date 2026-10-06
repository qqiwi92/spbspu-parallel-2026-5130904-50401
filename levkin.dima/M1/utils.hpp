#ifndef CIRCLE_HPP
#define CIRCLE_HPP

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
  bool gotIn(levkin::Circle c, int x, int y);
  bool gotIn(levkin::CircleVec cv, int x, int y);

}
#endif
