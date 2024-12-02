#pragma once

#include <tuple>
#include <vector>

#include "Grammar.h"

extern const std::vector<
    std::tuple<const char*, Grammar, std::vector<const char*>, std::vector<const char*>>>
    test_grammars;
