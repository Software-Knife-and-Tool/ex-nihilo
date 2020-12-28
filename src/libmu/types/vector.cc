/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  vector.cc: library vectors
 **
 **/
#include "libmu/types/vector.h"

#include <cassert>

#include "libmu/env.h"
#include "libmu/print.h"
#include "libmu/read.h"
#include "libmu/type.h"

#include "libmu/types/char.h"
#include "libmu/types/cons.h"

#include "libmu/types/exception.h"
#include "libmu/types/float.h"
#include "libmu/types/function.h"
#include "libmu/types/stream.h"

namespace libmu {
namespace core {

/** * view of vector object **/
TagPtr Vector::ViewOf(Env* env, TagPtr vector) {
  assert(IsType(vector));

  auto view =
      std::vector<TagPtr>{Symbol::Keyword("vector"), vector,
                          Fixnum(ToUint64(vector) >> 3).tag_, VecType(vector)};

  return Vector(env, view).tag_;
}

/** * garbage collection **/
void Vector::GcMark(Env* env, TagPtr vec) {
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
        Vector::vector_iter<TagPtr> iter(vec);
        for (auto it = iter.begin(); it != iter.end(); it = ++iter)
          env->GcMark(env, *it);
        break;
      }
      default:
        assert(!"vector type botch");
    }
  }
}

/** * allocate a general vector from the heap **/
Vector::Vector(Env* env, std::vector<TagPtr> src) {
  auto vp = env->heap_alloc<Layout>(
      sizeof(Layout) + (src.size() * sizeof(TagPtr)), SYS_CLASS::VECTOR);

  vp->type = SYS_CLASS::T;
  vp->length = src.size();

  tag_ = Entag(vp, TAG::EXTEND);

  std::memcpy(Vector::DataAddress<TagPtr>(tag_), src.data(),
              src.size() * sizeof(TagPtr));
}

/** * allocate a char vector from the heap **/
Vector::Vector(Env* env, std::string src) {
  if (src.size() <= IMMEDIATE_STR_MAX) {
    tag_ = String::MakeImmediate(src);
  } else {
    auto vp = env->heap_alloc<Layout>(
        sizeof(Layout) + (src.size() * sizeof(char)), SYS_CLASS::STRING);

    vp->type = SYS_CLASS::CHAR;
    vp->length = src.size();

    tag_ = Entag(vp, TAG::EXTEND);

    std::memcpy(Vector::DataAddress<char>(tag_), src.data(),
                src.size() * sizeof(char));
  }
}

/** * allocate a byte vector from the heap **/
Vector::Vector(Env* env, std::vector<uint8_t> src) {
  auto vp = env->heap_alloc<Layout>(
      sizeof(Layout) + (src.size() * sizeof(uint8_t)), SYS_CLASS::VECTOR);

  vp->type = SYS_CLASS::BYTE;
  vp->length = src.size();

  tag_ = Entag(vp, TAG::EXTEND);

  std::memcpy(Vector::DataAddress<uint8_t>(tag_), src.data(),
              src.size() * sizeof(uint8_t));
}

/** * allocate a fixnum vector from the heap **/
Vector::Vector(Env* env, std::vector<int64_t> src) {
  auto vp = env->heap_alloc<Layout>(
      sizeof(Layout) + (src.size() * sizeof(int64_t)), SYS_CLASS::VECTOR);

  vp->type = SYS_CLASS::FIXNUM;
  vp->length = src.size();

  tag_ = Entag(vp, TAG::EXTEND);

  std::memcpy(Vector::DataAddress<int64_t>(tag_), src.data(),
              src.size() * sizeof(int64_t));
}

/** * allocate a float vector from the heap **/
Vector::Vector(Env* env, std::vector<float> src) {
  auto vp = env->heap_alloc<Layout>(
      sizeof(Layout) + (src.size() * sizeof(float)), SYS_CLASS::VECTOR);

  vp->type = SYS_CLASS::FLOAT;
  vp->length = src.size();

  tag_ = Entag(vp, TAG::EXTEND);

  std::memcpy(Vector::DataAddress<float>(tag_), src.data(),
              src.size() * sizeof(float));
}

/** * vector parser **/
TagPtr Vector::Read(Env* env, TagPtr stream) {
  assert(Stream::IsType(stream));

  auto vectype = core::ReadForm(env, stream);

  if (!Symbol::IsKeyword(vectype))
    Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "type botch in vector reader", vectype);

  return ListToVector(env, vectype, Cons::Read(env, stream));
}

} /* namespace core */
} /* namespace libmu */
