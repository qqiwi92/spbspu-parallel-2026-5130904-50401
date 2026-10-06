#include "utils.hpp"
#include <istream>

namespace levkin
{
  std::istream &operator<<(std::istream &is, Circle &c)
  {
    int dummy;
    is >> c.radius >> dummy >> c.x >> c.y;

    return is;
  }

  bool gotIn(levkin::Circle c, int x, int y)
  {
    int dx = c.x - x;
    int dy = c.y - y;
  
    return dx * dx + dy * dy <= c.radius * c.radius;
  }
  
  bool gotIn(levkin::CircleVec cv, int x, int y)
  {
    bool result = true;
    for (levkin::Circle c : cv)
    {
      result = result && gotIn(c, x, y);
    }
    return result;
  }
}
