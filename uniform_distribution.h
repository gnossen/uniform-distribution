#include <iterator>
#include <list>
#include <limits>
#include <type_traits>

namespace gnossen {
namespace uniform_distribution {

namespace internal {
  template <typename Container>
  unsigned int score(const Container& container, size_t length) {
    unsigned int sum = 0;
    for (size_t i = 1; i < length - 1; ++i) {
      sum += container[i + 1] + container[i - 1] - 2 * container[i];
    }
    return sum;
  }

  class Combinations {
  public:
    Combinations(size_t collection_size, size_t selection_size) :
      collection_size_(collection_size),
      selection_size_(selection_size) {}

    class Iterator {
    public:
      using iterator_category = std::forward_iterator_tag;
      using value_type = std::list<size_t>;
      using difference_type = ptrdiff_t;
      using pointer = const value_type*;
      using reference = const value_type&;

      Iterator(const Combinations* container) :
        container_(container),
        combination_(),
        end_(false)
      {
        for (unsigned int i = 0;
             i <= container_->collection_size_ - container_->selection_size_;
             ++i)
        {
          combination_.push_back(i);
        }
      }

      Iterator(const Combinations* container, bool) :
        container_(container),
        combination_(),
        end_(true)
      {}

      bool update_last() {
        size_t last = combination_.back();
        combination_.pop_back();
        size_t next = get_next(last + 1);
        if (next == std::numeric_limits<size_t>::max()) {
          return false;
        } else {
          combination_.push_back(next);
          return true;
        }
      }

      bool push_new() {
        size_t next = get_next(0);
        if (next == std::numeric_limits<size_t>::max()) {
          return false;
        } else {
          combination_.push_back(next);
          return true;
        }
      }

      inline bool full() const {
        return combination_.size() == container_->selection_size_;
      }

      Iterator& operator++() {
        while (!update_last()) {
          if (combination_.size() == 0) {
            end_ = true;
            return *this;
          }
        }
        while (!full()) { push_new(); }
        return *this;
      }

      inline value_type operator*() {
        return combination_;
      }

      inline bool operator==(const Iterator& other) const {
        return container_ == other.container_ && (end_ == other.end_ || combination_ == other.combination_);
      }

      inline bool operator!=(const Iterator& other) const {
        return !(this->operator==(other));
      }

    private:

      // TODO: This could probably be better if we used an unordered_set.
      // Returns the next number at least as high as `i` that isn't.
      // already represented in the current combination. If no such number
      // exists, then returns std::numeric_limits<size_t>::max().
      inline size_t get_next(size_t i) const {
        for (auto element : combination_) {
          if (element == i)
            ++i;
        }
        if (i == container_->collection_size_) {
          return std::numeric_limits<size_t>::max();
        }
        return i;
      }

      const Combinations* container_;
      std::list<size_t> combination_;
      bool end_;
    };

    friend class Iterator;

    Iterator cbegin() const {
      return Iterator(this);
    }

    Iterator begin() const {
      return cbegin();
    }

    Iterator cend() const {
      return Iterator(this, true);
    }

    Iterator end() const {
      return cend();
    }

  private:
    size_t collection_size_;
    size_t selection_size_;
  };
}

template <typename T>
class OutputContainer {
public:

  // TODO: Needed?
  // using ElemType = T::value_type;

  // container must remain valid for the lifetime of the OutputContainer,
  // but to_remove is copied and may be mutated or deallocated as soon
  // as the call to the ctor ends.
  // length is supplied here in case the container type is such that calculating
  // this value is expensive.
  OutputContainer(const T* container, const std::list<size_t> to_remove) :
    container_(container),
    to_remove_(to_remove)
     {}

  class Iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename T::value_type;
    using difference_type = ptrdiff_t;
    using pointer = const value_type*;
    using reference = const value_type&;

    Iterator(const OutputContainer<T>* container, size_t position) :
      container_(container),
      base_position_(0),
      to_remove_position_(0),
      base_iterator_(container->container_->cbegin()),
      to_remove_iterator_(container->to_remove_.cbegin())
    {
      // Get to_remove_iterator_ in position
      if (position == std::size(*(container_->container_)) - std::size(container->to_remove_)) {
        // No need to update if this is just an end iterator.
        base_position_ = std::size(*(container_->container_));
        to_remove_position_ = std::size(container_->to_remove_);
        return;
      }
      maybe_skip();
      while (base_position_ - to_remove_position_ != position) {
        this->operator++();
      }
    }

    inline Iterator& operator++() {
      ++base_iterator_;
      ++base_position_;
      maybe_skip();
      return *this;
    }

    inline value_type operator*() const {
      return *base_iterator_;
    }

    inline bool operator==(const Iterator& other) const {
      return container_ == other.container_ && base_position_ == other.base_position_;
    }

    inline bool operator!=(const Iterator& other) const {
      return !(this->operator==(other));
    }

  private:

    void maybe_skip() {
      while (base_position_ == *to_remove_iterator_) {
        ++to_remove_iterator_;
        ++to_remove_position_;
        ++base_iterator_;
        ++base_position_;
      }
    }

    const OutputContainer<T>* container_;

    size_t base_position_;
    size_t to_remove_position_;
    typename T::const_iterator base_iterator_;
    typename std::list<size_t>::const_iterator to_remove_iterator_;

  };

  friend class Iterator;

  inline Iterator cbegin() const {
    return Iterator(this, 0);
  }

  inline Iterator begin() const {
    return cbegin();
  }

  inline Iterator cend() const {
    return Iterator(this, std::size(*container_) - std::size(to_remove_));
  }

  inline Iterator end() const {
    return cend();
  }

private:
  const T* container_;
  const std::list<size_t> to_remove_;
};

template <typename T, typename Container>
bool operator==(const OutputContainer<T> self, const Container& other) {
  auto other_it = other.cbegin();
  for (auto it = self.cbegin(); it != self.cend(); ++it) {
    if (other_it == other.cend()) return false;
    if (*it != *(other_it++)) return false;
  }
  if (other_it != other.cend()) return false;
  return true;
}



// template <typename T>
// OutputContainer<T> prune_uniform_naiive(const T& input, size_t output_size) {
//   static_assert(std::is_same<T::value_type, size_t>::value, "Container element type must be size_t.");
//   size_t length = std::size(input);
//   std::list<size_t> best_combination;
//   unsigned int best_score = std::numeric_limits<unsigned int>::max;
//   for (const std::list<size_t>& combination : internal::Combinations(length - 2, output_size)) {
//     // NOTE: We can't remove the first or the last, so we get combinations on
//     // the range [0, length - 2) and then add 1 to get the proper index range:
//     // [1, length - 1).
//     const std::list<size_t> to_remove;
//     std::transform(combination.cbegin(), combination.cend(),
//         std::back_inserter(to_remove),
//         [] (size_t i) -> size_t { return i + 1; });
//     unsigned int score = internal::score(OutputContainer<T>(&input, to_remove, length), length);
//     if (score < best_score) {
//       best_score = score;
//       best_combination = to_remove;
//     }
//   }
//   return OutputContainer<T>(&input, best_combination, length);
// }

// template <typename T>
// OutputContainer<T> prune_uniform_analytic(const T& input, size_t output_size) {
// 
// }

} // end namespace uniform_distribution.
} // end namespace gnossen.
