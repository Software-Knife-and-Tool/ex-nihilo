/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  vector.h: library vectors
 **
 **/
#if !defined(LIBMU_TYPES_VECTOR_H_)
#define LIBMU_TYPES_VECTOR_H_

#include <cassert>
#include <cstring>
#include <functional>
#include <memory>
#include <vector>

#include "libmu/heap/heap.h"
#include "libmu/type.h"

#include "libmu/types/fixnum.h"

namespace libmu {
namespace core {

using heap::Heap;

class Env;

/** vector class type **/
class Vector : public Type {
 private:
  typedef struct {
    SYS_CLASS type; /* vector type */
    size_t length;  /* length of vector in units */
    uint64_t base;  /* base of data */
  } HeapLayout;

  HeapLayout vector_;
  std::unique_ptr<std::vector<uint64_t>> hImage_;

 public: /* Tag */
  /** * accessors **/
  static const size_t MAX_LENGTH = 1024;

  static auto type(Tag vec) -> SYS_CLASS {
    return Untag<HeapLayout>(vec)->type;
  }

  static auto length(Tag vec) -> size_t {
    return Untag<HeapLayout>(vec)->length;
  }

  static auto base(Tag vec) -> uint64_t { return Untag<HeapLayout>(vec)->base; }

  static auto base(Tag vec, uint64_t base) -> uint64_t {
    Untag<HeapLayout>(vec)->base = base;
    return base;
  }

  static auto Map(Env*, Tag, Tag) -> Tag;
  static auto MapC(Env*, Tag, Tag) -> void;

  static constexpr auto IsType(Tag ptr) -> bool {
    return (IsExtended(ptr) && Heap::SysClass(ptr) == SYS_CLASS::VECTOR) ||
           (IsExtended(ptr) && Heap::SysClass(ptr) == SYS_CLASS::STRING) ||
           (IsImmediate(ptr) && ImmediateClass(ptr) == IMMEDIATE_CLASS::STRING);
  }

  template <typename T>
  static auto Data(Tag& vector) -> T* {
    assert(IsType(vector));

    return IsImmediate(vector)
               ? reinterpret_cast<T*>(reinterpret_cast<char*>(&vector) + 1)
               : reinterpret_cast<T*>(base(vector));
  }

  template <typename T>
  static auto Ref(Tag& vector, Tag index) -> T {
    assert(IsType(vector));

    return Data<T>(vector)[Fixnum::Uint64Of(index)];
  }

  static constexpr auto Length(Tag vec) -> size_t {
    assert(IsType(vec));

    return (IsImmediate(vec) && ImmediateClass(vec) == IMMEDIATE_CLASS::STRING)
               ? ImmediateSize(vec)
               : Untag<HeapLayout>(vec)->length;
  }

  static auto TypeOf(Tag vec) -> SYS_CLASS {
    assert(IsType(vec));

    return (IsImmediate(vec) && ImmediateClass(vec) == IMMEDIATE_CLASS::STRING)
               ? SYS_CLASS::CHAR
               : Untag<HeapLayout>(vec)->type;
  }

  static auto VecType(Tag vec) -> Tag {
    assert(IsType(vec));

    return Type::MapClassSymbol(TypeOf(vec));
  }

  static auto ListToVector(Env*, Tag, Tag) -> Tag;

  static auto GcMark(Env*, Tag) -> void;
  static auto Print(Env*, Tag, Tag, bool) -> void;
  static auto Read(Env*, Tag) -> Tag;
  static auto ViewOf(Tag) -> Tag;

 public: /* type model */
  auto Evict(Env*) -> Tag { return tag_; }
  static auto EvictTag(Env*, Tag) -> Tag;

  explicit Vector(std::vector<char>);
  explicit Vector(std::vector<float>);
  explicit Vector(std::vector<uint8_t>);
  explicit Vector(std::vector<uint64_t>);
  explicit Vector(std::vector<Tag>);

  /** * vector iterator **/
  template <typename V>
  struct vector_iter {
    typedef V* iterator;

    iterator vector_;
    std::ptrdiff_t length_;
    iterator current_;

    /* because the vector might be an immediate, we need to ref it */
    /* fix: figure out how to const the ref */
    vector_iter(Tag& vec)
        : vector_(Data<V>(vec)), length_(Length(vec)), current_(begin()) {}

    iterator begin() { return length_ ? vector_ : end(); }
    iterator end() { return nullptr; }

    iterator& operator++() { /* operator++ */
      std::ptrdiff_t curlen = current_ - vector_;

      if (curlen < (length_ - 1))
        ++current_;
      else
        current_ = end();

      return *&current_;
    }

    iterator operator++(int) { /* ++operator */

      iterator retn = current_;

      operator++();
      return retn;
    }

    V operator*() { return *current_; }
  };

}; /* class Vector */

/** * typed vectors **/
template <class V>
class VectorT : public Vector {
 private:
  std::vector<V> src_;

}; /* class VectorT */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TYPES_VECTOR_H_ */
