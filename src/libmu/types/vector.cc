/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  vector.cc: library vectors
 **
 **/
#include "libmu/types/vector.h"

#include <cassert>

#include "libmu/core.h"
#include "libmu/env.h"
#include "libmu/tagformat.h"
#include "libmu/type.h"

#include "libmu/heap/heap.h"

#include "libmu/types/char.h"
#include "libmu/types/cons.h"

#include "libmu/types/condition.h"
#include "libmu/types/float.h"
#include "libmu/types/function.h"
#include "libmu/types/stream.h"

namespace libmu {
namespace core {
/** * evict vector to heap **/
auto Vector::Evict(Env* env) -> Tag {
  auto hp = env->heap_alloc<Layout>(
      sizeof(Layout) + TagFormat<Layout>::HeapWords(vector_.length) * 8,
      SYS_CLASS::VECTOR);

  *hp = vector_;
  hp->base = reinterpret_cast<uint64_t>(
      (char*)hp + TagFormat<Layout>::HeapWords(sizeof(Layout)) * 8);

  std::memcpy(hp + TagFormat<Layout>::HeapWords(sizeof(Layout)) * 8,
              srcTag_.data(), vector_.length * 8);

  tag_ = Entag(hp, TAG::EXTEND);

  return tag_;
}

auto Vector::EvictTag(Env* env, Tag vector) -> Tag {
  assert(IsType(vector));
  assert(!Env::IsEvicted(env, vector));

  Layout* hl = Untag<Layout>(vector);
  auto hp = env->heap_alloc<Layout>(sizeof(Layout) + length(vector) * 8,
                                    SYS_CLASS::VECTOR);

  *hp = *hl;
  hp->base = reinterpret_cast<uint64_t>(
      (char*)hp + TagFormat<Layout>::HeapWords(sizeof(Layout)) * 8);

  std::memcpy(hp + TagFormat<Layout>::HeapWords(sizeof(Layout)) * 8,
              reinterpret_cast<char*>(base(vector)), length(vector) * 8);

  // env->heap_->DumpHeapImage<Layout>(Entag(hp, TAG::EXTEND));

  return Entag(hp, TAG::EXTEND);
}

/** * view of vector object **/
auto Vector::ViewOf(Env* env, Tag vector) -> Tag {
  assert(IsType(vector));

  auto view = std::vector<Tag>{
      Symbol::Keyword("vector"),          vector,
      Fixnum(ToUint64(vector) >> 3).tag_, VecType(vector),
      Fixnum(length(vector)).tag_,        Fixnum(base(vector)).tag_};

  return Vector(env, view).tag_;
}

/** * garbage collection **/
auto Vector::GcMark(Env* env, Tag vec) -> void {
  assert(IsType(vec));

  if (!Type::IsImmediate(vec) && !TagFormat<Layout>::IsGcMarked(vec)) {
    TagFormat<Layout>::GcMark(vec);

    switch (Vector::TypeOf(vec)) {
      case SYS_CLASS::BYTE:
      case SYS_CLASS::CHAR:
      case SYS_CLASS::FIXNUM:
      case SYS_CLASS::FLOAT:
        break;
      case SYS_CLASS::T: {
        Vector::vector_iter<Tag> iter(vec);
        for (auto it = iter.begin(); it != iter.end(); it = ++iter)
          env->GcMark(env, *it);
        break;
      }
      default:
        assert(!"vector type botch");
    }
  }
}

/** * vector parser **/
auto Vector::Read(Env* env, Tag stream) -> Tag {
  assert(Stream::IsType(stream));

  auto vectype = core::ReadForm(env, stream);

  if (!Symbol::IsKeyword(vectype))
    Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "type botch in vector reader", vectype);

  return ListToVector(env, vectype, Cons::Read(env, stream));
}

/** * allocate a general vector from the machine heap **/
Vector::Vector(Env*, std::vector<Tag> src) {
  srcTag_ = src;

  vector_.type = SYS_CLASS::T;
  vector_.length = src.size();
  vector_.base = reinterpret_cast<uint64_t>(src.data());

  tagFormat_ = new TagFormat<Layout>(SYS_CLASS::VECTOR, TAG::EXTEND, &vector_);
  tag_ = tagFormat_->tag_;
}

/** * allocate a char vector from the heap **/
Vector::Vector(Env*, std::vector<char> src) {
  if (src.size() <= IMMEDIATE_STR_MAX) {
    tag_ = String::MakeImmediate(std::string(src.begin(), src.end()));
  } else {
    srcChar_ = src;

    vector_.type = SYS_CLASS::CHAR;
    vector_.length = src.size();
    vector_.base = reinterpret_cast<uint64_t>(srcChar_.data());

    tagFormat_ =
        new TagFormat<Layout>(SYS_CLASS::VECTOR, TAG::EXTEND, &vector_);
    tag_ = tagFormat_->tag_;
  }
}

/** * allocate a byte vector from the heap **/
Vector::Vector(Env*, std::vector<uint8_t> src) {
  srcByte_ = src;

  vector_.type = SYS_CLASS::BYTE;
  vector_.length = src.size();
  vector_.base = reinterpret_cast<uint64_t>(srcByte_.data());

  tagFormat_ = new TagFormat<Layout>(SYS_CLASS::VECTOR, TAG::EXTEND, &vector_);
  tag_ = tagFormat_->tag_;
}

/** * allocate a fixnum vector from the heap **/
Vector::Vector(Env*, std::vector<int64_t> src) {
  srcFixnum_ = src;

  vector_.type = SYS_CLASS::FIXNUM;
  vector_.length = src.size();
  vector_.base = reinterpret_cast<uint64_t>(srcFixnum_.data());

  tagFormat_ = new TagFormat<Layout>(SYS_CLASS::VECTOR, TAG::EXTEND, &vector_);
  tag_ = tagFormat_->tag_;
}

/** * allocate a float vector from the heap **/
Vector::Vector(Env*, std::vector<float> src) {
  srcFloat_ = src;

  vector_.type = SYS_CLASS::FLOAT;
  vector_.length = src.size();
  vector_.base = reinterpret_cast<uint64_t>(srcFloat_.data());

  tagFormat_ = new TagFormat<Layout>(SYS_CLASS::VECTOR, TAG::EXTEND, &vector_);
  tag_ = tagFormat_->tag_;
}

} /* namespace core */
} /* namespace libmu */
