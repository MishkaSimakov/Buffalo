#pragma once

#include "LRTableBuilder.h"
#include "Parser.h"

class LRParser {
  static constexpr char cWordEndSymbol =
      LRParserDetails::LRTableBuilder::cWordEndSymbol;
  using Action = LRParserDetails::Action;

  LRParserDetails::LRTableBuilder::ActionsTableT actions_;
  LRParserDetails::LRTableBuilder::GotoTableT goto_;

  explicit LRParser(LRParserDetails::LRTableBuilder builder);

 public:
  static LRParser fit(Grammar grammar);

  bool predict(std::string_view word) const;
};

static_assert(IsParser<LRParser>);
