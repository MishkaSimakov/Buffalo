#pragma once

#include <list>
#include <string_view>
#include <variant>

class NonTerminal {
  static size_t id_counter;
  size_t id_;

 public:
  NonTerminal() : id_(id_counter++) {}

  size_t get_id() const { return id_; }

  ssize_t as_number() const { return -static_cast<ssize_t>(id_ + 1); }

  bool operator==(const NonTerminal&) const = default;
};

template <>
struct std::hash<NonTerminal> {
  size_t operator()(NonTerminal value) const noexcept {
    return std::hash<size_t>()(value.get_id());
  }
};

class Terminal {
  std::string_view string_;

 public:
  explicit Terminal(std::string_view string) : string_(string) {}

  std::string_view get_string() const { return string_; }

  bool operator==(const Terminal&) const = default;
};

class GrammarProductionResult {
 public:
  using PartT = std::variant<Terminal, NonTerminal>;
  static constexpr size_t cTerminalIndex = 0;
  static constexpr size_t cNonTerminalIndex = 1;

 protected:
  std::list<PartT> parts_;

 public:
  GrammarProductionResult() = default;
  GrammarProductionResult(Terminal part) : parts_({part}) {}
  GrammarProductionResult(NonTerminal part) : parts_({part}) {}

  static GrammarProductionResult empty() { return {}; }

  const std::list<PartT>& get_parts() const { return parts_; }

  GrammarProductionResult& operator+=(GrammarProductionResult other) {
    parts_.splice(parts_.end(), std::move(other.parts_));
    return *this;
  }

  struct const_iterator {
   private:
    std::list<PartT>::const_iterator list_iterator_;
    size_t part_iterator_;

    const_iterator(std::list<PartT>::const_iterator list_iterator,
                   size_t part_iterator)
        : list_iterator_(list_iterator), part_iterator_(part_iterator) {}

   public:
    const_iterator operator++() {
      if (list_iterator_->index() == cNonTerminalIndex) {
        part_iterator_ = 0;
        ++list_iterator_;
        return *this;
      }

      std::string_view view =
          std::get<cTerminalIndex>(*list_iterator_).get_string();

      if (view.size() == part_iterator_ + 1) {
        part_iterator_ = 0;
        ++list_iterator_;
      } else {
        ++part_iterator_;
      }

      return *this;
    }

    const_iterator operator++(int) {
      auto copy = *this;
      ++*this;
      return copy;
    }

    const_iterator operator--() {
      if (list_iterator_->index() == cNonTerminalIndex) {
        part_iterator_ = 0;
        --list_iterator_;
        return *this;
      }

      if (part_iterator_ == 0) {
        part_iterator_ = 0;
        --list_iterator_;
      } else {
        --part_iterator_;
      }

      return *this;
    }

    bool operator==(const const_iterator&) const = default;

    bool is_terminal() const {
      return list_iterator_->index() == cTerminalIndex;
    }

    char access_terminal() const {
      return std::get<cTerminalIndex>(*list_iterator_)
          .get_string()[part_iterator_];
    }

    NonTerminal access_nonterminal() const {
      return std::get<cNonTerminalIndex>(*list_iterator_);
    }

    // 0 is reserved for rule end so this function doesn't return it
    ssize_t as_number() const {
      if (is_terminal()) {
        return access_terminal();
      }

      return access_nonterminal().as_number();
    }

    friend GrammarProductionResult;
  };

  const_iterator cbegin() const { return const_iterator{parts_.cbegin(), 0}; }
  const_iterator cend() const { return const_iterator{parts_.cend(), 0}; }
};

inline GrammarProductionResult operator+(GrammarProductionResult left,
                                         GrammarProductionResult right) {
  left += std::move(right);
  return left;
}

inline size_t NonTerminal::id_counter = 0;
