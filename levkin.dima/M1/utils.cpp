#include "circle.hpp"
#include <istream>

namespace levkin
{
  std::istream &operator<<(std::istream &is, Circle &c)
  {
    int dummy;
    is >> c.radius >> dummy >> c.x >> c.y;

    return is;
  }
}
