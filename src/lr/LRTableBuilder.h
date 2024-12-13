#pragma once
#include <iostream>

#include "Grammar.h"

namespace LRParserDetails {
struct Position {
  NonTerminal from;
  const GrammarProductionResult& production;
  GrammarProductionResult::const_iterator iterator;

  Position(NonTerminal from, const GrammarProductionResult& production)
      : from(from), production(production), iterator(production.cbegin()) {}

  bool operator==(const Position& other) const {
    // iterator contains pointer to list element in it,
    // this pointer is unique for different productions
    return iterator == other.iterator;
  }

  GrammarProductionResult::const_iterator end_iterator() const {
    return production.cend();
  }

  std::string toString() const {
    std::string result;

    result += std::to_string(from.get_id()) + " -> ";

    for (auto itr = production.cbegin(); itr != production.cend(); ++itr) {
      if (itr == iterator) {
        result += '.';
      }

      if (itr.is_terminal()) {
        result += itr.access_terminal();
      } else {
        result += std::to_string(itr.access_nonterminal().get_id());
      }
    }

    if (iterator == production.cend()) {
      result += '.';
    }

    return result;
  }
};
}  // namespace LRParserDetails

template <>
struct std::hash<LRParserDetails::Position> {
  size_t operator()(const LRParserDetails::Position& position) const {
    return std::hash<GrammarProductionResult::const_iterator>()(
        position.iterator);
  }
};

namespace LRParserDetails {
using State = std::unordered_map<Position, std::unordered_set<char>>;

struct StateInfo {
  size_t index;
  std::unordered_map<ssize_t, size_t> gotos;
};

struct RejectAction {};
struct AcceptAction {};
struct ReduceAction {
  NonTerminal next;
  size_t remove_count;
};
struct ShiftAction {
  size_t next_state;
};

using Action =
    std::variant<RejectAction, AcceptAction, ReduceAction, ShiftAction>;

struct Conflict {
  State state;
  char symbol;
  std::vector<Action> actions;

  Conflict(State state, char symbol, std::vector<Action> actions)
      : state(std::move(state)), symbol(symbol), actions(std::move(actions)) {}
};

struct ActionsConflictException : std::exception {
  std::vector<Conflict> conflicts;

  explicit ActionsConflictException(std::vector<Conflict> conflicts)
      : conflicts(std::move(conflicts)) {}

  const char* what() const noexcept override {
    return "conflicting actions in LRTableBuilder";
  }
};

class LRTableBuilder {
  constexpr static auto states_hasher_fn = [](const State& state) {
    return unordered_range_hasher_fn(
        state |
        std::views::transform(
            [](const std::pair<Position, std::unordered_set<char>>& element) {
              size_t following_hash = unordered_range_hasher_fn(element.second);
              return tuple_hasher_fn(element.first, following_hash);
            }));
  };

  const Grammar& grammar_;

  std::unordered_map<NonTerminal, std::unordered_set<char>> first_;
  std::unordered_map<NonTerminal, std::unordered_set<char>> follow_;
  std::unordered_map<State, StateInfo, decltype(states_hasher_fn)> states_;
  std::vector<std::unordered_map<ssize_t, size_t>> goto_;
  std::vector<std::vector<Action>> actions_;

  static std::unordered_map<ssize_t, State> group_by_next(const State& state);

  void build_first_table();
  void build_follow_table();
  void build_states_table();

  State closure(State state) const;

 public:
  using ActionsTableT = decltype(actions_);
  using GotoTableT = decltype(goto_);

  static constexpr char cWordEndSymbol = 'z' + 1;
  static constexpr size_t cSymbolsCount = cWordEndSymbol - 'a' + 1;

  explicit LRTableBuilder(const Grammar& grammar);

  auto& get_actions_table() { return actions_; }
  auto& get_first_table() { return first_; }
  auto& get_follow_table() { return follow_; }
  auto& get_goto_table() { return goto_; }
};
}  // namespace LRParserDetails

inline std::ostream& operator<<(std::ostream& os,
                                const LRParserDetails::Action& action) {
  using namespace LRParserDetails;

  if (std::holds_alternative<AcceptAction>(action)) {
    os << "accept";
  } else if (std::holds_alternative<RejectAction>(action)) {
    os << "reject";
  } else if (std::holds_alternative<ShiftAction>(action)) {
    ShiftAction shift = std::get<ShiftAction>(action);
    os << "shift{" << shift.next_state << "}";
  } else {
    ReduceAction reduce = std::get<ReduceAction>(action);
    os << "reduce{" << reduce.next.get_id() << ", " << reduce.remove_count
       << "}";
  }

  return os;
}

inline std::ostream& operator<<(std::ostream& os,
                                const LRParserDetails::State& state) {
  for (const auto& [position, follow] : state) {
    os << position.toString() << " [ ";
    for (char symbol : follow) {
      os << symbol << " ";
    }
    os << "]" << std::endl;
  }

  return os;
}
