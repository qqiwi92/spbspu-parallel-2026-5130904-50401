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
    for (BorderCircle c : cv)
    {
      result = result && gotIn(c.circle, x, y);
    }
    return result;
  }

  BorderCircleVec getData(std::istream &is)
  {

    Circle dummyCircle;
    BorderCircleVec dataVec;

    while (is >> dummyCircle)
    {
      dataVec.emplace_back(dummyCircle, getBorderSquare(dummyCircle));
    }

    if (!is.eof())
    {
      throw std::invalid_argument("bad input file");
    }
    return dataVec;
  }
  size_t runTests(size_t howManyTests, size_t seed, BorderCircleVec const &cv)
  {
    std::mt19937 gen(seed);
    size_t result = 0;
    
    for (size_t testCount = 0; testCount < howManyTests; ++testCount)
    {
      for (size_t i = 0; i < cv.size(); ++i)
      {
        BorderRange xRng = cv[i].xRange;
        BorderRange yRng = cv[i].yRange;

        std::uniform_int_distribution< int > xDist(xRng[0], xRng[1]);
        std::uniform_int_distribution< int > yDist(yRng[0], yRng[1]);

        int randX = xDist(gen);
        int randY = xDist(gen);
        result += gotIn(cv[i].circle, randX, randY);
      }
    }
    return result;
  }
}
