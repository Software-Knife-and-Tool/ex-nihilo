/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
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
#include "libmu/types/condition.h"
#include "libmu/types/struct.h"

namespace libmu {
namespace mu {

using Condition = core::Condition;
using Struct = core::Struct;
using Frame = core::Env::Frame;
using Type = core::Type;

/** * (structp obj) => bool **/
auto IsStruct(Frame* fp) -> void {
  fp->value = Type::Bool(Struct::IsType(fp->argv[0]));
}

/** * (struct keyword vector) => object **/
auto MakeStruct(Frame* fp) -> void {
  auto name = fp->argv[0];
  auto values = fp->argv[1];

  if (!core::Symbol::IsKeyword(name))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "struct",
                     name);

  if (!core::Cons::IsList(values))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "struct",
                     values);

  fp->value = Struct(name, values).Evict(fp->env);
}

/** * implements (struct-type struct) => symbol **/
auto StructType(Frame* fp) -> void {
  auto strct = fp->argv[0];

  if (!Struct::IsType(strct))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "struct-type", strct);

  fp->value = Struct::stype(strct);
}

/** * (struct-values struct) => vector */
auto StructValues(Frame* fp) -> void {
  auto strct = fp->argv[0];

  if (!Struct::IsType(strct))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "struct-slots", strct);

  fp->value = Struct::slots(strct);
}

} /* namespace mu */
} /* namespace libmu */
