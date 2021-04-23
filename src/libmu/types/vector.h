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

#include "libmu/tagformat.h"
#include "libmu/type.h"

#include "libmu/heap/heap.h"
#include "libmu/types/fixnum.h"

namespace libmu {
namespace core {

using heap::Heap;

/** * Vector type class **/
class Vector : public Type {
 public:
  typedef struct {
    SYS_CLASS type; /* vector type */
    size_t length;  /* length of vector in units */
    uint64_t base;  /* base of data */
  } Layout;

 public:
  Layout vector_;
  TagFormat<Layout>* tagFormat_;

  std::vector<Tag> srcTag_;
  std::vector<char> srcChar_;
  std::vector<float> srcFloat_;
  std::vector<uint8_t> srcByte_;
  std::vector<int64_t> srcFixnum_;

 public: /* Tag */
  /** * accessors **/
  static const size_t MAX_LENGTH = 1024;

  static auto type(Tag vec) -> SYS_CLASS { return Untag<Layout>(vec)->type; }
  static auto length(Tag vec) -> size_t { return Untag<Layout>(vec)->length; }
  static auto base(Tag vec) -> uint64_t { return Untag<Layout>(vec)->base; }

  static auto base(Tag vec, uint64_t base) -> uint64_t {
    Untag<Layout>(vec)->base = base;
    return base;
  }

  static auto Map(Env*, Tag, Tag) -> Tag;
  static auto MapC(Env*, Tag, Tag) -> void;

  static constexpr auto IsType(Tag ptr) -> bool {
    return (IsExtended(ptr) &&
            TagFormat<Layout>::SysClass(ptr) == SYS_CLASS::VECTOR) ||
           (IsExtended(ptr) &&
            TagFormat<Layout>::SysClass(ptr) == SYS_CLASS::STRING) ||
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
               : Untag<Layout>(vec)->length;
  }

  static auto TypeOf(Tag vec) -> SYS_CLASS {
    assert(IsType(vec));

    return (IsImmediate(vec) && ImmediateClass(vec) == IMMEDIATE_CLASS::STRING)
               ? SYS_CLASS::CHAR
               : Untag<Layout>(vec)->type;
  }

  static auto VecType(Tag vec) -> Tag {
    assert(IsType(vec));

    return Type::MapClassSymbol(TypeOf(vec));
  }

  static auto ListToVector(Env*, Tag, Tag) -> Tag;

  static auto GcMark(Env*, Tag) -> void;
  static auto Print(Env*, Tag, Tag, bool) -> void;
  static auto Read(Env*, Tag) -> Tag;
  static auto ViewOf(Env*, Tag) -> Tag;

 public: /* type model */
  auto Evict(Env*) -> Tag;
  static auto EvictTag(Env*, Tag) -> Tag;

  explicit Vector(Tag tag) {
    tagFormat_ = nullptr;
    tag_ = tag;
  }

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

/** * VectorT type class **/
template <SYS_CLASS sc, typename V>
class VectorT : public Vector {
private:
  std::vector<V> src_;

 public: /* type model */
  explicit VectorT(Env*, std::vector<Tag> src) {
    src_ = src;

    vector_.type = sc;
    vector_.length = src.size();
    vector_.base = reinterpret_cast<uint64_t>(src.data());

    tagFormat_ = new TagFormat<Layout>(SYS_CLASS::VECTOR, TAG::EXTEND, &vector_);
    tag_ = tagFormat_->tag_;
  }
}; /* class VectorT */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TYPES_VECTOR_H_ */
