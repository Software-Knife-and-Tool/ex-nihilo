/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu-vector.cc: library vector functions
 **
 **/
#include <cassert>

#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/types/char.h"
#include "libmu/types/condition.h"
#include "libmu/types/cons.h"
#include "libmu/types/float.h"
#include "libmu/types/function.h"
#include "libmu/types/string.h"
#include "libmu/types/vector.h"

namespace libmu {
namespace mu {

using Condition = core::Condition;
using Fixnum = core::Fixnum;
using Frame = core::Env::Frame;
using Type = core::Type;
using Vector = core::Vector<Type::Tag>;

/** * (vectorp form) => bool**/
auto IsVector(Frame* fp) -> void {
  fp->value = Type::Bool(Vector::IsType(fp->argv[0]));
}

/** * (vector-refx vector) => object **/
auto VectorRef(Frame* fp) -> void {
  auto vector = fp->argv[0];
  auto index = fp->argv[1];

  if (!Vector::IsType(vector))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "svref",
                     vector);

  if (!Fixnum::IsType(index))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "svref",
                     index);

  if (Fixnum::Int64Of(index) < 0)
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "svref",
                     index);

  if (Fixnum::Uint64Of(index) >= Vector::Length(vector))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::RANGE_ERROR, "svchar",
                     index);

  switch (Vector::TypeOf(vector)) {
    case Type::SYS_CLASS::FIXNUM:
      fp->value = Fixnum(Vector::Ref<int64_t>(vector, index)).tag_;
      break;
    case Type::SYS_CLASS::BYTE:
      fp->value = Fixnum(Vector::Ref<uint8_t>(vector, index)).tag_;
      break;
    case Type::SYS_CLASS::CHAR:
      fp->value = core::Char(Vector::Ref<char>(vector, index)).tag_;
      break;
    case Type::SYS_CLASS::FLOAT:
      fp->value = core::Float(Vector::Ref<float>(vector, index)).tag_;
      break;
    case Type::SYS_CLASS::T:
      fp->value = Vector::Ref<Type::Tag>(vector, index);
      break;
    default:
      assert(!"vector type botch");
  }
}

/** * (svlength vector) => fixnum **/
auto VectorLength(Frame* fp) -> void {
  auto vector = fp->argv[0];

  if (!Vector::IsType(vector))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "svlength", vector);

  fp->value = Fixnum(Vector::Length(vector)).tag_;
}

/** * (svtype vector) => symbol **/
auto VectorType(Frame* fp) -> void {
  if (!Vector::IsType(fp->argv[0]))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "svtype)",
                     fp->argv[0]);

  fp->value = Vector::VecType(fp->argv[0]);
}

/** * (vector-map function vector) => vector **/
auto VectorMap(Frame* fp) -> void {
  auto func = fp->argv[0];
  auto vector = fp->argv[1];

  if (!core::Function::IsType(func))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "svmap",
                     func);

  if (!Vector::IsType(vector))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "svmap",
                     vector);

  fp->value = Vector::Map(fp->env, func, vector);
}

/** * (vector-mapc function vector) => vector **/
auto VectorMapC(Frame* fp) -> void {
  auto func = fp->argv[0];
  auto vector = fp->argv[1];

  if (!core::Function::IsType(func))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "svmapc",
                     func);

  if (!Vector::IsType(vector))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "svmapc",
                     vector);

  Vector::MapC(fp->env, func, vector);
  fp->value = vector;
}

/** * (vector type list) => vector **/
auto VectorCons(Frame* fp) -> void {
  auto type = fp->argv[0];
  auto list = fp->argv[1];

  if (!core::Symbol::IsType(type))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "vector",
                     type);

  if (!core::Cons::IsList(list))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "vector",
                     list);

  fp->value = Vector::ListToVector(fp->env, type, list);
}

} /* namespace mu */
} /* namespace libmu */
