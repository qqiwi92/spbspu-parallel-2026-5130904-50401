#ifndef UTILS_HPP
#define UTILS_HPP

#include <array>
#include <istream>
#include <ostream>
#include <vector>
namespace levkin
{
  using BorderRange = std::array< int, 2 >;
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
    BorderRange xRange;
    BorderRange yRange;

    BorderCircle(Circle const &c, std::array< BorderRange, 2 > const &ranges);
  };

  struct BorderCircleVec
  {
    struct Ranges
    {
      BorderRange xRange;
      BorderRange yRange;
    };

    std::vector< Circle > circles{};
    Ranges ranges;

    void updateRanges();
    size_t rangeArea() const;
  };
  std::istream &operator>>(std::istream &is, Circle &c);

  BorderCircleVec getData(std::istream &is);
  bool gotIn(levkin::Circle c, int x, int y);
  bool gotIn(levkin::BorderCircleVec cv, int x, int y);
  size_t runTests(size_t howManyTests, size_t seed, BorderCircleVec const &cv);
  std::array< BorderRange, 2 > getBorderSquare(Circle const &c);

}
#endif
