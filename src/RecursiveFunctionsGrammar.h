#pragma once

#include "Grammar.h"
#include "GrammarBuilder.h"
#include "SyntaxNode.h"

#ifdef COMPILING_GRAMMAR
constexpr bool cIsCompilingGrammar = true;
#else
constexpr bool cIsCompilingGrammar = false;
#endif

extern const GrammarBuilder<SyntaxNode, cIsCompilingGrammar> cRecursiveFunctionsGrammar;
