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
}
#endif
