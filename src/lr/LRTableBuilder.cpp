#include "LRTableBuilder.h"

#include <iostream>

#include "Hashers.h"

std::unordered_map<ssize_t, LRParserDetails::State>
LRParserDetails::LRTableBuilder::group_by_next(const State& state) {
  std::unordered_map<ssize_t, State> result;

  for (const auto& [position, follow] : state) {
    auto copy = position;

    ssize_t next_id;

    if (copy.iterator == copy.end_iterator()) {
      next_id = 0;
    } else {
      next_id = copy.iterator.as_number();
      ++copy.iterator;
    }

    result[next_id].emplace(copy, follow);
  }

  return result;
}

void LRParserDetails::LRTableBuilder::build_first_table() {
  auto update_data = [this] {
    std::unordered_map<NonTerminal, std::unordered_set<char>> update;

    for (const auto& [non_terminal, productions] : grammar_.get_productions()) {
      for (const auto& production : productions) {
        if (production.is_empty()) {
          continue;
        }

        if (production.cbegin().is_terminal()) {
          update[non_terminal].insert(production.cbegin().access_terminal());
        } else {
          // we encountered non-terminal
          const auto& symbols =
              first_[production.cbegin().access_nonterminal()];
          for (char symbol : symbols) {
            if (!first_[non_terminal].contains(symbol)) {
              update[non_terminal].insert(symbol);
            }
          }
        }
      }

      auto itr = update.find(non_terminal);

      if (itr != update.end()) {
        std::erase_if(itr->second, [this, &non_terminal](char symbol) {
          return first_[non_terminal].contains(symbol);
        });

        if (itr->second.empty()) {
          update.erase(itr);
        }
      }
    }

    return update;
  };

  auto curr_added = update_data();
  while (!curr_added.empty()) {
    // merge into result
    for (auto& [non_terminal, terminals] : curr_added) {
      first_[non_terminal].merge(terminals);
    }

    // find new data
    curr_added = update_data();
  }
}

void LRParserDetails::LRTableBuilder::build_follow_table() {
  auto update_data = [this] {
    std::unordered_map<NonTerminal, std::unordered_set<char>> update;

    for (const auto& [non_terminal, productions] : grammar_.get_productions()) {
      for (const auto& production : productions) {
        const auto& parts = production.get_parts();
        for (auto itr = parts.begin(); itr != parts.end(); ++itr) {
          if (std::holds_alternative<Terminal>(*itr)) {
            continue;
          }

          auto curr_non_terminal = std::get<NonTerminal>(*itr);
          auto next = std::next(itr);
          std::unordered_set<char> new_follow;

          if (next == parts.end()) {
            new_follow = follow_[non_terminal];
          } else if (std::holds_alternative<Terminal>(*next)) {
            new_follow = {std::get<Terminal>(*next).get_string().front()};
          } else {
            // next is non-terminal
            new_follow = first_[std::get<NonTerminal>(*next)];
          }

          for (char symbol : new_follow) {
            if (!follow_[curr_non_terminal].contains(symbol)) {
              update[curr_non_terminal].insert(symbol);
            }
          }
        }
      }
    }

    return update;
  };

  std::unordered_map<NonTerminal, std::unordered_set<char>> curr_added;
  curr_added[grammar_.get_start()].insert(cWordEndSymbol);

  while (!curr_added.empty()) {
    // merge into result
    for (auto& [non_terminal, follows] : curr_added) {
      follow_[non_terminal].merge(follows);
    }

    // find new data
    curr_added = update_data();
  }
}

void LRParserDetails::LRTableBuilder::build_states_table() {
  // building goto table
  Position start_position(grammar_.get_start(),
                          grammar_.get_start_productions().front());

  // store State + it's index
  // then all states will be replaced with their indices
  std::vector<decltype(states_)::iterator> updated;

  auto [start_itr, _] = states_.emplace(
      closure({{start_position, {cWordEndSymbol}}}), StateInfo{0, {}});
  updated.emplace_back(start_itr);

  while (!updated.empty()) {
    auto current_itr = updated.back();
    const auto& [current, info] = *current_itr;
    updated.pop_back();

    // do all possible goto's
    auto grouped = group_by_next(current);

    for (auto& [next_id, state] : grouped) {
      if (next_id == 0) {
        continue;
      }

      state = closure(state);

      auto itr = states_.find(state);
      if (itr == states_.end()) {
        // this is new state, we should process it in the next iterations
        StateInfo new_info{states_.size(), {}};
        std::tie(itr, std::ignore) =
            states_.emplace(std::move(state), std::move(new_info));
        updated.emplace_back(itr);
      }

      current_itr->second.gotos[next_id] = itr->second.index;
    }
  }

  goto_.resize(states_.size());
  for (const auto& [index, gotos] : states_ | std::views::values) {
    for (const auto& [from, to] : gotos) {
      goto_[index][from] = to;
    }
  }
}

LRParserDetails::State LRParserDetails::LRTableBuilder::closure(
    State state) const {
  State result;
  State curr_updated = std::move(state);

  while (!curr_updated.empty()) {
    auto prev_updated = curr_updated;
    for (auto& [position, following] : curr_updated) {
      result[position].merge(following);
    }
    curr_updated.clear();

    for (const auto& updated_position : prev_updated | std::views::keys) {
      if (updated_position.iterator == updated_position.end_iterator() ||
          updated_position.iterator.is_terminal()) {
        continue;
      }

      auto non_terminal = updated_position.iterator.access_nonterminal();
      auto following_itr = std::next(updated_position.iterator);

      std::unordered_set<char> new_following;
      if (following_itr == updated_position.end_iterator()) {
        new_following = follow_.at(updated_position.from);
      } else if (following_itr.is_terminal()) {
        new_following = {following_itr.access_terminal()};
      } else {
        // following_itr is non-terminal
        new_following = first_.at(following_itr.access_nonterminal());
      }

      for (const auto& production_result :
           grammar_.get_productions_for(non_terminal)) {
        Position new_position(non_terminal, production_result);

        // meticulously merge new position into resulting state
        auto updated_following =
            new_following |
            std::views::filter([&result, &new_position](char symbol) {
              auto itr = result.find(new_position);

              if (itr == result.end()) {
                return true;
              }

              return !itr->second.contains(symbol);
            });

        curr_updated[new_position].insert(updated_following.begin(),
                                          updated_following.end());
      }
    }

    erase_if(
        curr_updated,
        [](const std::pair<const Position, std::unordered_set<char>>& value) {
          return value.second.empty();
        });
  }

  return result;
}

LRParserDetails::LRTableBuilder::LRTableBuilder(const Grammar& grammar)
    : grammar_(grammar) {
  build_first_table();
  build_follow_table();
  build_states_table();

  auto state_by_index = [this](size_t index) {
    return std::ranges::find_if(states_, [index](const auto& pair) {
      return pair.second.index == index;
    });
  };

  // for (size_t i = 0; i < states_.size(); ++i) {
  // const auto& [state, info] = *state_by_index(i);
  //   std::cout << "state #" << i << std::endl;
  //   std::cout << state << std::endl;
  //
  //   std::cout << "gotos:" << std::endl;
  //   for (auto [from, to] : info.gotos) {
  //     if (from < 0) {
  //       std::cout << -(from + 1);
  //     } else {
  //       std::cout << static_cast<char>(from);
  //     }
  //     std::cout << " -> #" << to << std::endl;
  //   }
  //   std::cout << std::endl;
  // }

  // build actions according to goto
  std::vector<std::vector<std::vector<Action>>> temp_actions;
  temp_actions.resize(states_.size());

  for (const auto& [state, info] : states_) {
    auto& actions = temp_actions[info.index];
    actions.resize(cSymbolsCount);

    auto grouped = group_by_next(state);

    for (const auto& [position, follow] : grouped[0]) {
      Action action;

      if (position.from == grammar_.get_start()) {
        action = AcceptAction{};
      } else {
        size_t remove_count = position.production.size();
        NonTerminal next = position.from;

        action = ReduceAction{next, remove_count};
      }

      for (char symbol : follow) {
        actions[symbol - 'a'].push_back(action);
      }
    }

    for (char symbol = 'a'; symbol <= cWordEndSymbol; ++symbol) {
      if (grouped.contains(symbol)) {
        size_t next_state = info.gotos.at(symbol);
        actions[symbol - 'a'].emplace_back(ShiftAction{next_state});
      }

      if (actions[symbol - 'a'].empty()) {
        actions[symbol - 'a'].emplace_back(RejectAction{});
      }
    }
  }

  std::vector<Conflict> conflicts;

  actions_.resize(states_.size());
  for (size_t state_id = 0; state_id < states_.size(); ++state_id) {
    const auto& state_actions = temp_actions[state_id];
    actions_[state_id].resize(cSymbolsCount);

    for (char symbol = 'a'; symbol <= cWordEndSymbol; ++symbol) {
      if (state_actions[symbol - 'a'].size() != 1) {
        conflicts.emplace_back(state_by_index(state_id)->first, symbol,
                               state_actions[symbol - 'a']);
        continue;
      }

      actions_[state_id][symbol - 'a'] = state_actions[symbol - 'a'].front();
    }
  }

  if (!conflicts.empty()) {
    throw ActionsConflictException(std::move(conflicts));
  }
}
