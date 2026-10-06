#include "utils.hpp"
#include <array>
#include <istream>
#include <random>
#include <vector>

namespace levkin
{
  std::istream &operator<<(std::istream &is, Circle &c)
  {
    int dummy;
    is >> c.radius >> dummy >> c.x >> c.y;

    return is;
  }
  BorderCircle::BorderCircle(Circle const &c, std::array< p_t, 2 > const &ranges):
      circle(c),
      xRange(ranges[0]),
      yRange(ranges[1])
  {
  }

std::array< p_t, 2 > getBorderSquare(Circle const &c)
{
  std::array< p_t, 2 > result;
  result[0].x = c.x - c.radius;
  result[0].y = c.y - c.radius;
  result[1].x = c.x + c.radius;
  result[1].y = c.y + c.radius;

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
  for (Circle c : cv)
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
  for (size_t i = 0; i < howManyTests; ++i)
  {
    result += gotIn(cv, 0, 0);
  }
}
}
