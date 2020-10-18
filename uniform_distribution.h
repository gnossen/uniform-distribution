#include <iterator>
#include <list>
#include <limits>
#include <type_traits>

namespace gnossen {
namespace uniform_distribution {

// namespace internal {
//   template <typename Container>
//   unsigned int score(const Container& container, size_t length) {
//     // TODO: Implement.
//   }
// 
//   class Combinations {
//     Combinations(size_t collection_size, size_t selection_size) :
//       collection_size_(collection_size),
//       selection_size_(selection_size) {}
// 
//     class Iterator {
// 
//     };
// 
//     Iterator cbegin() const {}
//     Iterator cend() const {}
// 
//   private:
// 
//   };
// }

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
//   for (const std::list<size_t>& to_remove : internal::Combinations(length, output_size)) {
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
