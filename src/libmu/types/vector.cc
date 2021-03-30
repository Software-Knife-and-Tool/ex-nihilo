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

/** evict vector to heap **/
auto Vector::Evict(Env* env) -> Tag {
  auto hp = env->heap_alloc<HeapLayout>(sizeof(HeapLayout), SYS_CLASS::VECTOR);

  *hp = vector_;
  // evict data

  tag_ = Entag(hp, TAG::EXTEND);

  return tag_;
}

auto Vector::EvictTag(Env* env, Tag vector) -> Tag {
  assert(IsType(vector));
  assert(!Env::IsEvicted(env, vector));

  // printf("EvictTag: vector\n");
  auto hp = env->heap_alloc<HeapLayout>(sizeof(HeapLayout), SYS_CLASS::VECTOR);
  auto sp = Untag<HeapLayout>(vector);

  *hp = *sp;

  // evict data
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

  if (!Type::IsImmediate(vec) && !env->heap_->IsGcMarked(vec)) {
    env->heap_->GcMark(vec);

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
  size_t nalloc = sizeof (Heap::HeapInfo) + (((src.size() * 8) + 7) & ~7);
  uint64_t hInfo = Heap::MakeHeapInfo(nalloc, SYS_CLASS::VECTOR);

  auto hImage = std::make_unique<std::vector<uint64_t>>(1 + src.size());
  hImage->at(0) = hInfo;
  std::memcpy(hImage->data() + 1, src.data(), src.size() * sizeof (Tag));

  vector_.type = SYS_CLASS::T;
  vector_.length = src.size();
  vector_.base = reinterpret_cast<uint64_t>(hImage->data() + 1);

  tag_ = Entag(hImage->data() + 1, TAG::EXTEND);
}

/** * allocate a char vector from the heap **/
Vector::Vector(Env* env, std::vector<char> srcv) {
  std::string src(srcv.begin(), srcv.end());

  // auto v = VectorT<char, SYS_CLASS::CHAR>(env, srcv);
  tag_ = Vector(env, src).tag_;
}

/** * allocate a char vector from the heap **/
Vector::Vector(Env* env, const std::string& src) {
  if (src.size() <= IMMEDIATE_STR_MAX) {
    tag_ = String::MakeImmediate(src);
  } else {
    auto vp = env->heap_alloc<HeapLayout>(
        sizeof(HeapLayout) + (src.size() * sizeof(char)), SYS_CLASS::STRING);

    vp->type = SYS_CLASS::CHAR;
    vp->length = src.size();
    vp->base = reinterpret_cast<uint64_t>(vp) + sizeof(HeapLayout);

    tag_ = Entag(vp, TAG::EXTEND);

    std::memcpy(Vector::Data<char>(tag_), src.data(),
                src.size() * sizeof(char));
  }
}

/** * allocate a byte vector from the heap **/
Vector::Vector(Env* env, std::vector<uint8_t> src) {
  auto vp = env->heap_alloc<HeapLayout>(
      sizeof(HeapLayout) + (src.size() * sizeof(uint8_t)), SYS_CLASS::VECTOR);

  vp->type = SYS_CLASS::BYTE;
  vp->length = src.size();
  vp->base = reinterpret_cast<uint64_t>(vp) + sizeof(HeapLayout);

  tag_ = Entag(vp, TAG::EXTEND);

  std::memcpy(Vector::Data<uint8_t>(tag_), src.data(),
              src.size() * sizeof(uint8_t));
}

/** * allocate a fixnum vector from the heap **/
Vector::Vector(Env* env, std::vector<int64_t> src) {
  auto vp = env->heap_alloc<HeapLayout>(
      sizeof(HeapLayout) + (src.size() * sizeof(int64_t)), SYS_CLASS::VECTOR);

  vp->type = SYS_CLASS::FIXNUM;
  vp->length = src.size();
  vp->base = reinterpret_cast<uint64_t>(vp) + sizeof(HeapLayout);

  tag_ = Entag(vp, TAG::EXTEND);

  std::memcpy(Vector::Data<int64_t>(tag_), src.data(),
              src.size() * sizeof(int64_t));
}

/** * allocate a float vector from the heap **/
Vector::Vector(Env* env, std::vector<float> src) {
  auto vp = env->heap_alloc<HeapLayout>(
      sizeof(HeapLayout) + (src.size() * sizeof(float)), SYS_CLASS::VECTOR);

  vp->type = SYS_CLASS::FLOAT;
  vp->length = src.size();
  vp->base = reinterpret_cast<uint64_t>(vp) + sizeof(HeapLayout);

  tag_ = Entag(vp, TAG::EXTEND);

  std::memcpy(Vector::Data<float>(tag_), src.data(),
              src.size() * sizeof(float));
}

} /* namespace core */
} /* namespace libmu */
