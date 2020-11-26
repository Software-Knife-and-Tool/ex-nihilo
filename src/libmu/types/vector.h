/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
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

class Env;

/** vector class type **/
class Vector : public Type {
 private:
  typedef struct {
    SYS_CLASS type;
    size_t length;
  } Layout;

 public: /* TagPtr */
  static const size_t MAX_LENGTH = 1024;

  static TagPtr Map(Env*, TagPtr, TagPtr);
  static void MapC(Env*, TagPtr, TagPtr);

  static constexpr bool IsType(TagPtr ptr) {
    return (IsExtended(ptr) && Heap::SysClass(ptr) == SYS_CLASS::VECTOR) ||
           (IsExtended(ptr) && Heap::SysClass(ptr) == SYS_CLASS::STRING) ||
           (IsImmediate(ptr) && ImmediateClass(ptr) == IMMEDIATE_CLASS::STRING);
  }

 public: /* iterator */
  /* fix: figure out how to const the ref */
  template <typename T>
  static T* DataAddress(TagPtr& vector) {
    assert(IsType(vector));

    return IsImmediate(vector)
               ? reinterpret_cast<T*>(reinterpret_cast<char*>(&vector) + 1)
               : reinterpret_cast<T*>(
                     reinterpret_cast<char*>(Untag<Layout>(vector)) +
                     sizeof(Layout));
  }

  /* figure out how to const the ref */
  template <typename T>
  static T Ref(TagPtr& vector, TagPtr index) {
    assert(IsType(vector));

    return DataAddress<T>(vector)[Fixnum::Uint64Of(index)];
  }

  static constexpr size_t Length(TagPtr vec) {
    assert(IsType(vec));

    return (IsImmediate(vec) && ImmediateClass(vec) == IMMEDIATE_CLASS::STRING)
               ? ImmediateSize(vec)
               : Untag<Layout>(vec)->length;
  }

  static SYS_CLASS TypeOf(TagPtr vec) {
    assert(IsType(vec));

    return (IsImmediate(vec) && ImmediateClass(vec) == IMMEDIATE_CLASS::STRING)
               ? SYS_CLASS::CHAR
               : Untag<Layout>(vec)->type;
  }

  static TagPtr VecType(TagPtr vec) {
    assert(IsType(vec));

    return Type::MapClassSymbol(TypeOf(vec));
  }

  static TagPtr ListToVector(Env*, TagPtr, TagPtr);
  static TagPtr Read(Env*, TagPtr);
  static TagPtr ViewOf(Env*, TagPtr);

  static void GcMark(Env*, TagPtr);
  static void PrintVector(Env*, TagPtr, TagPtr, bool);

 public: /* object model */
  TagPtr Evict(Env*, const char*) { return tag_; }

  explicit Vector(Env*, std::string);          /* string */
  explicit Vector(Env*, std::vector<TagPtr>);  /* general */
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
    vector_iter(TagPtr& vec)
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

} /* namespace libmu */

#endif /* LIBMU_TYPES_VECTOR_H_ */
