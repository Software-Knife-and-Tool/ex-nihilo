/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu-condition.cc: library condition functions
 **
 **/
#include <cassert>
#include <iostream>

#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/types/condition.h"
#include "libmu/types/function.h"
#include "libmu/types/namespace.h"

namespace libmu {
namespace mu {

using Cons = core::Cons;
using Condition = core::Condition;
using Frame = core::Env::Frame;
using Function = core::Function;
using Symbol = core::Symbol;
using Type = core::Type;

/** * mu function (condition? object) => bool **/
void IsCondition(Frame* fp) {
  fp->value = Type::Bool(Condition::IsType(fp->argv[0]));
}

/** * mu function (raise string object) never returns **/
[[noreturn]] void Raise(Frame* fp) {
  auto reason = fp->argv[0];
  auto object = fp->argv[1];

  if (!core::String::IsType(reason))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "error",
                     reason);

  Condition::Raise(fp->env, Condition::CONDITION_CLASS::SIMPLE_ERROR,
                   core::String::StdStringOf(reason), object);
}

/** * (raise-condition condition) **/
[[noreturn]] void RaiseCondition(Frame* fp) {
  auto condition = fp->argv[0];

  if (!Condition::IsType(condition))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "raise-condition", condition);

  throw condition;
}

/** * (condition tag source) **/
void MakeCondition(Frame* fp) {
  auto tag = fp->argv[0];
  auto reason = fp->argv[1];
  auto source = fp->argv[2];

  if (!Symbol::IsKeyword(tag))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "condition", tag);

  if (!core::String::IsType(reason))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "condition", reason);

  fp->value = Condition(tag, core::Env::LastFrame(fp->env), source, reason)
                  .Evict(fp->env);
}

/** * (with-condition func func) **/
void WithCondition(Frame* fp) {
  auto thunk = fp->argv[0];
  auto handler = fp->argv[1];
  auto mark = fp->env->frames_.size();

  if (!Function::IsType(thunk))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "with-condition", thunk);

  if (!Function::IsType(handler))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "with-condition", handler);

  Type::Tag value;

  try {
    value = Function::Funcall(fp->env, thunk, std::vector<Type::Tag>{});
  } catch (Type::Tag ex) {
    fp->env->frames_.resize(mark);
    value = Function::Funcall(fp->env, handler, std::vector<Type::Tag>{ex});
  } catch (...) {
    assert(!"unexpected throw from libmu");
  }

  fp->value = value;
}

/** * (block :symbol :func) => object **/
void Block(Frame* fp) {
  auto tag = fp->argv[0];
  auto fn = fp->argv[1];
  auto mark = fp->env->frames_.size();

  if (!Symbol::IsType(tag))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "is not a symbol (::block)", tag);

  if (!core::IsSpecOp(fn) && !Function::IsType(fn))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "is not a function (::block)", fn);

  try {
    fp->value =
        core::Eval(fp->env, Cons::List(fp->env, std::vector<Type::Tag>{fn}));
  } catch (
      Type::Tag ex) { /* think: don't we need a specific return condition? */
    if (Cons::IsType(ex) && Type::Eq(tag, Cons::car(ex))) {
      fp->env->frames_.resize(mark);
      fp->value = Cons::cdr(ex);
    } else
      throw ex;
  } catch (Condition& ex) {
    throw ex;
  }
}

/** * (return :keyword object) **/
void Return(Frame* fp) {
  auto tag = fp->argv[0];
  auto value = fp->argv[1];

  if (!Symbol::IsType(tag) || !Symbol::IsKeyword(tag))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "is not a symbol (%return)", tag);

  throw Cons(tag, value).Evict(fp->env);
}

} /* namespace mu */
} /* namespace libmu */
