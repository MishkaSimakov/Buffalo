#pragma once

struct is_nothrow {};

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
