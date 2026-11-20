#include "utils.hpp"

#include <array>
#include <iostream>
#include <istream>
#include <limits>
#include <random>
#include <vector>

namespace levkin {
  std::istream &operator>>(std::istream &is, Circle &c)
  {
    int dummy = 0;
    is >> c.radius >> dummy >> c.x >> c.y;

    return is;
  }

  void CirclesWithRanges::updateRanges()
  {
    if (circles.empty()) {
      ranges.x_range = {0, 0};
      ranges.y_range = {0, 0};
      return;
    }

    int min_x = std::numeric_limits< int >::max();
    int max_x = std::numeric_limits< int >::min();
    int min_y = std::numeric_limits< int >::max();
    int max_y = std::numeric_limits< int >::min();

    for (const Circle &c : circles) {
      const std::array< BorderRange, 2 > border_square = getBorderSquare(c);
      min_x = std::min(min_x, border_square[0][0]);
      max_x = std::max(max_x, border_square[0][1]);
      min_y = std::min(min_y, border_square[1][0]);
      max_y = std::max(max_y, border_square[1][1]);
    }

    ranges.x_range = {min_x, max_x};
    ranges.y_range = {min_y, max_y};
  }

  size_t CirclesWithRanges::rangeArea() const
  {
    return (ranges.x_range[1] - ranges.x_range[0]) * (ranges.y_range[1] - ranges.y_range[0]);
  }

  std::array< BorderRange, 2 > getBorderSquare(const Circle &c)
  {
    std::array< BorderRange, 2 > result{};
    result[0][0] = c.x - c.radius;
    result[0][1] = c.x + c.radius;
    result[1][0] = c.y - c.radius;
    result[1][1] = c.y + c.radius;

    return result;
  }

  bool gotIn(Circle c, double x, double y)
  {
    const double dx = c.x - x;
    const double dy = c.y - y;

    return dx * dx + dy * dy <= c.radius * static_cast< double >(c.radius);
  }

  AnyAllResult gotIn(const CirclesWithRanges &cv, double x, double y)
  {
    bool hit_any = false;
    bool hit_all = true;
    for (const Circle &c : cv.circles) {
      const bool hit = gotIn(c, x, y);
      hit_any = hit_any || hit;
      hit_all = hit_all && hit;
    }

    AnyAllResult result{};
    result.any = hit_any ? 1 : 0;
    result.all = hit_all ? 1 : 0;
    return result;
  }

  CirclesWithRanges getData(std::istream &is)
  {
    Circle dummy_circle{};
    CirclesWithRanges data_vec{};

    while (is >> dummy_circle) {
      data_vec.circles.emplace_back(dummy_circle);
    }

    if (!is.eof()) {
      throw std::invalid_argument("bad input file");
    }

    data_vec.updateRanges();
    return data_vec;
  }

  AnyAllResult runTests(size_t how_many_tests, size_t seed, const CirclesWithRanges &cv)
  {
    std::default_random_engine gen(seed);
    AnyAllResult result{};

    const BorderRange x_rng = cv.ranges.x_range;
    const BorderRange y_rng = cv.ranges.y_range;

    std::uniform_real_distribution< double > x_dist(x_rng[0], x_rng[1]);
    std::uniform_real_distribution< double > y_dist(y_rng[0], y_rng[1]);

    for (size_t test_count = 0; test_count < how_many_tests; ++test_count) {
      const double rand_x = x_dist(gen);
      const double rand_y = y_dist(gen);

      result += gotIn(cv, rand_x, rand_y);
    }

    return result;
  }
}
