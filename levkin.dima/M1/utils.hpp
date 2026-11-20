#ifndef UTILS_HPP
#define UTILS_HPP

#include <array>
#include <istream>
#include <vector>

namespace levkin {
  using BorderRange = std::array< int, 2 >;

  struct Circle {
    int x = 0;
    int y = 0;
    int radius = 0;
  };

  struct AnyAllResult {
    size_t any = 0;
    size_t all = 0;

    AnyAllResult &operator+=(const AnyAllResult &other)
    {
      any += other.any;
      all += other.all;
      return *this;
    }
  };

  struct CirclesWithRanges {
    struct Ranges {
      BorderRange x_range;
      BorderRange y_range;
    };

    std::vector< Circle > circles{};
    Ranges ranges;

    void updateRanges();
    size_t rangeArea() const;
  };

  std::istream &operator>>(std::istream &is, Circle &c);

  CirclesWithRanges getData(std::istream &is);
  bool gotIn(Circle c, double x, double y);
  AnyAllResult gotIn(const CirclesWithRanges &cv, double x, double y);
  AnyAllResult runTests(size_t how_many_tests, size_t seed, const CirclesWithRanges &cv);
  std::array< BorderRange, 2 > getBorderSquare(const Circle &c);
}

#endif
