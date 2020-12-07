/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
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
#include "libmu/types/cons.h"
#include "libmu/types/exception.h"
#include "libmu/types/float.h"
#include "libmu/types/function.h"
#include "libmu/types/string.h"
#include "libmu/types/vector.h"

namespace libmu {
namespace mu {

/** * mu function (vector? form) => bool**/
void IsVector(Env::Frame* fp) {
  fp->value = Type::GenBool(Vector::IsType(fp->argv[0]), fp->argv[0]);
}

/** * mu function (svref vector) => object **/
void VectorRef(Env::Frame* fp) {
  auto vector = fp->argv[0];
  auto index = fp->argv[1];

  if (!Vector::IsType(vector))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "svref",
                     vector);

  if (!Fixnum::IsType(index))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "svref",
                     index);

  if (Fixnum::Int64Of(index) < 0)
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "svref",
                     index);

  if (Fixnum::Uint64Of(index) >= Vector::Length(vector))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::RANGE_ERROR, "svchar",
                     index);

  switch (Vector::TypeOf(vector)) {
    case Type::SYS_CLASS::FIXNUM:
      fp->value = Fixnum(Vector::Ref<int64_t>(vector, index)).tag_;
      break;
    case Type::SYS_CLASS::BYTE:
      fp->value = Fixnum(Vector::Ref<uint8_t>(vector, index)).tag_;
      break;
    case Type::SYS_CLASS::CHAR:
      fp->value = Char(Vector::Ref<char>(vector, index)).tag_;
      break;
    case Type::SYS_CLASS::FLOAT:
      fp->value = Float(Vector::Ref<float>(vector, index)).tag_;
      break;
    case Type::SYS_CLASS::T:
      fp->value = Vector::Ref<TagPtr>(vector, index);
      break;
    default:
      assert(!"vector type botch");
  }
}

/** * mu function (svlength vector) => fixnum **/
void VectorLength(Env::Frame* fp) {
  auto vector = fp->argv[0];

  if (!Vector::IsType(vector))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "svlength",
                     vector);

  fp->value = Fixnum(Vector::Length(vector)).tag_;
}

/** * mu function (svtype vector) => symbol **/
void VectorType(Env::Frame* fp) {
  if (!Vector::IsType(fp->argv[0]))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "svtype)",
                     fp->argv[0]);

  fp->value = Vector::VecType(fp->argv[0]);
}

/** * mu function (svmap function vector) => vector **/
void VectorMap(Env::Frame* fp) {
  auto func = fp->argv[0];
  auto vector = fp->argv[1];

  if (!Function::IsType(func))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "svmap",
                     func);

  if (!Vector::IsType(vector))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "svmap",
                     vector);

  fp->value = Vector::Map(fp->env, func, vector);
}

/** * mu function (svmapc function vector) => vector **/
void VectorMapC(Env::Frame* fp) {
  auto func = fp->argv[0];
  auto vector = fp->argv[1];

  if (!Function::IsType(func))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "svmapc",
                     func);

  if (!Vector::IsType(vector))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "svmapc",
                     vector);

  Vector::MapC(fp->env, func, vector);
  fp->value = vector;
}

/** * (vector type list) => vector **/
void MakeVector(Env::Frame* fp) {
  auto type = fp->argv[0];
  auto list = fp->argv[1];

  if (!Symbol::IsType(type))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "vector",
                     type);

  if (!Cons::IsList(list))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "vector",
                     list);

  fp->value = Vector::ListToVector(fp->env, type, list);
}

} /* namespace mu */
} /* namespace libmu */
