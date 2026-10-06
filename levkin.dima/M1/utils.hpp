#ifndef UTILS_HPP
#define UTILS_HPP

#include <array>
#include <istream>
#include <ostream>
#include <vector>
namespace levkin
{
  struct p_t
  {
    int x, y;
  };

  struct Circle
  {
    int x, y, radius;
  };

  struct BorderCircle
  {
    Circle circle;
    p_t xRange;
    p_t yRange;
    
    BorderCircle(Circle const &c, std::array< p_t, 2 > const &ranges);
  };

  using BorderCircleVec = std::vector< BorderCircle >;
  std::istream &operator>>(std::istream &is, Circle &c);

  BorderCircleVec getData(std::istream &is);
  bool gotIn(levkin::Circle c, int x, int y);
  bool gotIn(levkin::BorderCircleVec cv, int x, int y);
  size_t runTests(size_t howManyTests, size_t seed, BorderCircleVec const &cv);
  std::array< p_t, 2 > getBorderSquare(Circle const &c);

}
#endif
