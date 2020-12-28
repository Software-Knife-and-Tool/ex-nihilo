/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu-struct.cc: library structs
 **
 **/
#include <cassert>

#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/types/char.h"
#include "libmu/types/exception.h"
#include "libmu/types/struct.h"

namespace libmu {
namespace mu {

using Exception = core::Exception;
using Struct = core::Struct;
using Frame = core::Env::Frame;
using Type = core::Type;

/** * mu function (struct? obj) => bool **/
void IsStruct(Frame* fp) {
  fp->value = Type::Bool(Struct::IsType(fp->argv[0]));
}

/** * mu function (struct keyword vector) => object **/
void MakeStruct(Frame* fp) {
  auto name = fp->argv[0];
  auto values = fp->argv[1];

  if (!core::Symbol::IsKeyword(name))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "struct",
                     name);

  if (!core::Cons::IsList(values))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "struct",
                     values);

  fp->value = Struct(name, values).Evict(fp->env);
}

/** * implements (struct-type struct) => symbol **/
void StructType(Frame* fp) {
  auto strct = fp->argv[0];

  if (!Struct::IsType(strct))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "struct-type", strct);

  fp->value = Struct::stype(strct);
}

/** * mu function (struct-values struct) => vector */
void StructValues(Frame* fp) {
  auto strct = fp->argv[0];

  if (!Struct::IsType(strct))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "struct-slots", strct);

  fp->value = Struct::slots(strct);
}

} /* namespace mu */
} /* namespace libmu */
