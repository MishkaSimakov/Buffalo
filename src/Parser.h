#pragma once
#include <string>

#include "Grammar.h"

template <typename T>
concept IsParser = requires(const T parser, Grammar grammar, std::string word) {
  { T::fit(grammar) } -> std::same_as<T>;
  { parser.predict(word) } -> std::same_as<bool>;
};
