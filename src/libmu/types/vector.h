/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
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
    SYS_CLASS type;
    size_t length;
  } HeapLayout;

 public: /* Tag */
  static const size_t MAX_LENGTH = 1024;

  static Tag Map(Env*, Tag, Tag);
  static void MapC(Env*, Tag, Tag);

  static constexpr bool IsType(Tag ptr) {
    return (IsExtended(ptr) && Heap::SysClass(ptr) == SYS_CLASS::VECTOR) ||
           (IsExtended(ptr) && Heap::SysClass(ptr) == SYS_CLASS::STRING) ||
           (IsImmediate(ptr) && ImmediateClass(ptr) == IMMEDIATE_CLASS::STRING);
  }

 public: /* iterator */
  /* fix: figure out how to const the ref */
  template <typename T>
  static T* DataAddress(Tag& vector) {
    assert(IsType(vector));

    return IsImmediate(vector)
               ? reinterpret_cast<T*>(reinterpret_cast<char*>(&vector) + 1)
               : reinterpret_cast<T*>(
                     reinterpret_cast<char*>(Untag<HeapLayout>(vector)) +
                     sizeof(HeapLayout));
  }

  /* figure out how to const the ref */
  template <typename T>
  static T Ref(Tag& vector, Tag index) {
    assert(IsType(vector));

    return DataAddress<T>(vector)[Fixnum::Uint64Of(index)];
  }

  static constexpr size_t Length(Tag vec) {
    assert(IsType(vec));

    return (IsImmediate(vec) && ImmediateClass(vec) == IMMEDIATE_CLASS::STRING)
               ? ImmediateSize(vec)
               : Untag<HeapLayout>(vec)->length;
  }

  static SYS_CLASS TypeOf(Tag vec) {
    assert(IsType(vec));

    return (IsImmediate(vec) && ImmediateClass(vec) == IMMEDIATE_CLASS::STRING)
               ? SYS_CLASS::CHAR
               : Untag<HeapLayout>(vec)->type;
  }

  static Tag VecType(Tag vec) {
    assert(IsType(vec));

    return Type::MapClassSymbol(TypeOf(vec));
  }

  static constexpr Tag VSpecOf(Tag t) { return t; }

  static Tag ListToVector(Env*, Tag, Tag);
  static Tag Read(Env*, Tag);
  static Tag ViewOf(Env*, Tag);

  static void GcMark(Env*, Tag);
  static void Print(Env*, Tag, Tag, bool);

 public: /* type model */
  Tag Evict(Env*) { return tag_; }

 public: /* object */
  explicit Vector(Tag t) : Type() { tag_ = t; }

  explicit Vector(Env*, const std::string&);   /* string */
  explicit Vector(Env*, std::vector<char>);    /* string */
  explicit Vector(Env*, std::vector<Tag>);     /* general */
  explicit Vector(Env*, std::vector<float>);   /* float */
  explicit Vector(Env*, std::vector<int64_t>); /* fixnum */
  explicit Vector(Env*, std::vector<uint8_t>); /* byte */

 public:
  /** * vector iterator **/
  template <typename T>
  struct vector_iter {
    typedef T* iterator;

    iterator vector_;
    std::ptrdiff_t length_;
    iterator current_;

    /* because the vector might be an immediate, we need to ref it */
    /* fix: figure out how to const the ref */
    vector_iter(Tag& vec)
        : vector_(DataAddress<T>(vec)),
          length_(Length(vec)),
          current_(begin()) {}

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

    T operator*() { return *current_; }
  };

}; /* class Vector */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TYPES_VECTOR_H_ */
