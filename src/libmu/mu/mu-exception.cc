/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu-exception.cc: library exception functions
 **
 **/
#include <cassert>
#include <iostream>

#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/types/exception.h"
#include "libmu/types/function.h"
#include "libmu/types/namespace.h"

namespace libmu {
namespace mu {

using Frame = Env::Frame;

/** * mu function (exception? object) => bool **/
void IsException(Frame* fp) {
  fp->value = Type::Bool(Exception::IsType(fp->argv[0]));
}

/** * mu function (raise string object) never returns **/
[[noreturn]] void Raise(Frame* fp) {
  auto reason = fp->argv[0];
  auto object = fp->argv[1];

  if (!String::IsType(reason))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "error",
                     reason);

  Exception::Raise(fp->env, Exception::EXCEPT_CLASS::SIMPLE_ERROR,
                   String::StdStringOf(reason), object);
}

/** * (raise-exception exception) **/
[[noreturn]] void RaiseException(Frame* fp) {
  auto exception = fp->argv[0];

  if (!Exception::IsType(exception))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "raise-exception", exception);

  throw exception;
}

/** * (exception tag source) **/
void MakeException(Frame* fp) {
  auto tag = fp->argv[0];
  auto reason = fp->argv[1];
  auto source = fp->argv[2];

  if (!Symbol::IsKeyword(tag))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "exception",
                     tag);

  if (!String::IsType(reason))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "exception",
                     reason);

  fp->value = Exception(tag, Env::LastFrame(fp->env), source, reason)
                  .Evict(fp->env, "mu-exception:make");
}

/** * (with-exception func func) **/
void WithException(Frame* fp) {
  auto thunk = fp->argv[0];
  auto handler = fp->argv[1];

  if (!Function::IsType(thunk))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "with-exception", thunk);

  if (!Function::IsType(handler))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "with-exception", handler);

  TagPtr value;

  int nframes = fp->env->frames_.size();

  try {
    value = Function::Funcall(fp->env, thunk, std::vector<TagPtr>{});
  } catch (TagPtr ex) {
    for (auto i = fp->env->frames_.size() - nframes; i != 0; --i)
      fp->env->frames_.pop_back();

    value = Function::Funcall(fp->env, handler, std::vector<TagPtr>{ex});
  } catch (...) {
    assert(!"unexpected throw from libmu");
  }

  fp->value = value;
}

/** * (block :symbol :func) => object **/
void Block(Frame* fp) {
  auto tag = fp->argv[0];
  auto fn = fp->argv[1];

  if (!Symbol::IsType(tag))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "is not a symbol (.block)", tag);

  if (!Function::IsType(fn))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "is not a function (.block)", fn);

  try {
    fp->value =
        core::Eval(fp->env, Cons::List(fp->env, std::vector<TagPtr>{fn}));
  } catch (TagPtr ex) {
    if (Cons::IsType(ex) && Type::Eq(tag, Cons::car(ex)))
      fp->value = Cons::cdr(ex);
    else
      throw ex;
  } catch (Exception& ex) {
    throw ex;
  }
}

/** * (return :keyword object) **/
void Return(Frame* fp) {
  auto tag = fp->argv[0];
  auto value = fp->argv[1];

  if (!Symbol::IsType(tag) || !Symbol::IsKeyword(tag))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "is not a symbol (%return)", tag);

  throw Cons(tag, value).Evict(fp->env, "mu-exception:return");
}

} /* namespace mu */
} /* namespace libmu */
