#ifndef UTILS_HPP
#define UTILS_HPP

#include <array>
#include <istream>
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
  struct TestResult
  {
    size_t hitsAll;
    size_t hitsAny;
  };

  struct AnyAllResult
  {
    size_t any, all;

    AnyAllResult &operator+=(const AnyAllResult &other)
    {
      any += other.any;
      all += other.all;
      return *this;
    }
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
  bool gotIn(levkin::Circle c, double x, double y);
  AnyAllResult gotIn(BorderCircleVec &cv, double x, double y);
  AnyAllResult runTests(size_t howManyTests, size_t seed, BorderCircleVec const &cv);
  std::array< BorderRange, 2 > getBorderSquare(Circle const &c);

}
#endif
