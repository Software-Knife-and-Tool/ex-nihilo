/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu-function.cc: library functions
 **
 **/
#include <cassert>
#include <memory>

#include "libmu/compiler.h"
#include "libmu/env.h"
#include "libmu/eval.h"
#include "libmu/print.h"
#include "libmu/type.h"
#include "libmu/types/cons.h"

#include "libmu/types/exception.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/function.h"
#include "libmu/types/symbol.h"
#include "libmu/types/vector.h"

namespace libmu {
namespace mu {

using Cons = core::Cons;
using Exception = core::Exception;
using Fixnum = core::Fixnum;
using Frame = core::Env::Frame;
using Function = core::Function;
using Type = core::Type;

/** * (function? object) => bool **/
void IsFunction(Frame* fp) {
  fp->value = Type::Bool(Function::IsType(fp->argv[0]));
}

/** * (trampoline thunk) => object **/
void Trampoline(Frame* fp) {
  fp->value = fp->argv[0];

  if (!Function::IsType(fp->value))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "trampoline",
                     fp->value);

  do {
    fp->value =
        Function::Funcall(fp->env, fp->value, std::vector<Type::TagPtr>{});
  } while (Function::IsType(fp->value));
}

/** * (closure function) => function **/
void Closure(Frame* fp) {
  auto fn = fp->argv[0];

  if (!Function::IsType(fn))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "closure",
                     fn);

  if (!Type::Null(Function::env(fn))) {
    std::vector<Frame*> context{};

    Cons::cons_iter<Type::TagPtr> iter(Function::env(fn));
    for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
      auto fn = it->car;
      auto offset = 0;
      auto lambda = Cons::car(Function::form(fn));
      size_t nargs = Function::arity(fn); /* think: rest args? */
      auto args = new Type::TagPtr[nargs];
      auto frame = fp->env->MapFrame(Function::frame_id(fn));

      Cons::cons_iter<Type::TagPtr> iter(Cons::car(lambda));
      for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
        args[offset] = frame->argv[offset];
        offset++;
      }

      context.push_back(
          new Frame(fp->env, Function::frame_id(fn), fn, args, nargs));
    }

    Function::context(fn, context);
  }

  fp->value = fn;
}

/** * (.frame-ref fixnum fixnum) => object **/
void FrameRef(Frame* fp) {
  auto frame_id = fp->argv[0];
  auto offset = fp->argv[1];

  if (!Fixnum::IsType(frame_id))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, ".frame-ref",
                     frame_id);

  if (!Fixnum::IsType(offset))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, ".frame-ref",
                     offset);

  auto lfp = fp->env->MapFrame(frame_id);

  fp->value = lfp->argv[Fixnum::Uint64Of(offset)];
}

/** * (letq frame-id offset value) => value **/
void Letq(Frame* fp) {
  auto frame_id = fp->argv[0];
  auto offset = fp->argv[1];
  auto value = fp->argv[2];

  if (!Fixnum::IsType(frame_id))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "letq",
                     frame_id);

  if (!Fixnum::IsType(offset))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "letq",
                     offset);

  auto lfp = fp->env->MapFrame(frame_id);

  lfp->argv[Fixnum::Uint64Of(offset)] = value;

  fp->value = value;
}

} /* namespace mu */
} /* namespace libmu */
