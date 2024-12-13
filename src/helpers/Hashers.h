#pragma once
#include <unordered_map>
#include <unordered_set>

template <typename... Args>
struct TupleHasher {
 private:
  struct InternalHasher {
    size_t current = 0;

    template <typename T>
    InternalHasher& operator<<(const T& value) {
      current ^=
          std::hash<T>()(value) + 0xeeffccdd + (current << 5) + (current >> 3);

      return *this;
    }
  };

 public:
  size_t operator()(const Args&... args) const {
    InternalHasher hasher{};
    (hasher << ... << args);
    return hasher.current;
  }
};

inline auto tuple_hasher_fn = []<typename... Args>(const Args&... args) {
  return TupleHasher<Args...>()(args...);
};

template <typename R,
          typename ElementHasher = std::hash<std::ranges::range_value_t<R>>>
struct UnorderedRangeHasher {
 private:
  static constexpr size_t cModulo = 1000003793;
  static constexpr size_t cBase = 239;

  static size_t fast_exponent(size_t value) {
    size_t result = 1;
    size_t base = cBase;

    while (value > 0) {
      if (value % 2 == 1) {
        result *= base;
        result %= cModulo;
      }

      base *= base;
      base %= cModulo;

      value /= 2;
    }

    return result;
  }

  [[no_unique_address]] ElementHasher hasher_;

 public:
  size_t operator()(const R& range) const {
    size_t result = 0;

    for (const auto& value : range) {
      size_t hash = hasher_(value);
      result ^= fast_exponent(hash);
    }

    return result;
  }
};

inline auto unordered_range_hasher_fn = []<typename R>(const R& range) {
  return UnorderedRangeHasher<R>()(range);
};
