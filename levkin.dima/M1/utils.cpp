#include "utils.hpp"
#include <array>
#include <iostream>
#include <istream>
#include <random>
#include <vector>
namespace levkin
{
  std::istream &operator>>(std::istream &is, Circle &c)
  {
    int dummy;
    is >> c.radius >> dummy >> c.x >> c.y;

    return is;
  }
  BorderCircle::BorderCircle(Circle const &c, std::array< BorderRange, 2 > const &ranges):
      circle(c),
      xRange(ranges[0]),
      yRange(ranges[1])
  {
  }

  void BorderCircleVec::updateRanges()
  {
    if (circles.empty())
    {
      ranges.xRange = {0, 0};
      ranges.yRange = {0, 0};
      return;
    }

    int minX = std::numeric_limits< int >::max();
    int maxX = std::numeric_limits< int >::min();
    int minY = std::numeric_limits< int >::max();
    int maxY = std::numeric_limits< int >::min();

    for (const Circle &c : circles)
    {
      std::array< BorderRange, 2 > borderSquare = getBorderSquare(c);
      minX = std::min(minX, borderSquare[0][0]);
      maxX = std::max(maxX, borderSquare[0][1]);
      minY = std::min(minY, borderSquare[1][0]);
      maxY = std::max(maxY, borderSquare[1][1]);
    }

    ranges.xRange = {minX, maxX};
    ranges.yRange = {minY, maxY};
  }
  size_t BorderCircleVec::rangeArea() const
  {
    return (ranges.xRange[1] - ranges.xRange[0]) * (ranges.yRange[1] - ranges.yRange[0]);
  }

  std::array< BorderRange, 2 > getBorderSquare(Circle const &c)
  {
    std::array< BorderRange, 2 > result;
    result[0][0] = c.x - c.radius;
    result[0][1] = c.x + c.radius;
    result[1][0] = c.y - c.radius;
    result[1][1] = c.y + c.radius;

    return result;
  }
  bool gotIn(Circle c, int x, int y)
  {
    int dx = c.x - x;
    int dy = c.y - y;

    return dx * dx + dy * dy <= c.radius * c.radius;
  }

  bool gotIn(BorderCircleVec cv, int x, int y)
  {
    bool result = true;
    for (Circle c : cv.circles)
    {
      result = result && gotIn(c, x, y);
    }
    return result;
  }

  BorderCircleVec getData(std::istream &is)
  {

    Circle dummyCircle;
    BorderCircleVec dataVec;

    while (is >> dummyCircle)
    {
      dataVec.circles.emplace_back(dummyCircle);
    }

    if (!is.eof())
    {
      throw std::invalid_argument("bad input file");
    }

    dataVec.updateRanges();
    return dataVec;
  }
  size_t runTests(size_t howManyTests, size_t seed, BorderCircleVec const &cv)
  {
    std::mt19937 gen(seed);
    size_t result = 0;

    BorderRange xRng = cv.ranges.xRange;
    BorderRange yRng = cv.ranges.yRange;

    std::uniform_int_distribution< int > xDist(xRng[0], xRng[1]);
    std::uniform_int_distribution< int > yDist(yRng[0], yRng[1]);
    for (size_t testCount = 0; testCount < howManyTests; ++testCount)
    {
      for (size_t i = 0; i < cv.circles.size(); ++i)
      {

        int randX = xDist(gen);
        int randY = xDist(gen);
        result += gotIn(cv.circles[i], randX, randY);
      }
    }
    return result;
  }
}
