/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
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
#include "libmu/core.h"
#include "libmu/env.h"
#include "libmu/type.h"
#include "libmu/types/cons.h"

#include "libmu/types/condition.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/function.h"
#include "libmu/types/symbol.h"
#include "libmu/types/vector.h"

namespace libmu {
namespace mu {

using Cons = core::Cons;
using Condition = core::Condition;
using Fixnum = core::Fixnum;
using Frame = core::Env::Frame;
using Function = core::Function;
using Type = core::Type;

/** * (functionp object) => bool **/
auto IsFunction(Frame* fp) -> void {
  fp->value = Type::Bool(Function::IsType(fp->argv[0]));
}

/** * (trampoline thunk) => object **/
auto Trampoline(Frame* fp) -> void {
  fp->value = fp->argv[0];

  if (!Function::IsType(fp->value))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "trampoline", fp->value);

  do {
    fp->value = Function::Funcall(fp->env, fp->value, std::vector<Type::Tag>{});
  } while (Function::IsType(fp->value));
}

/** * (closure function) => function **/
auto Closure(Frame* fp) -> void {
  auto fn = fp->argv[0];

  if (!Function::IsType(fn))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "closure",
                     fn);

  if (!Type::Null(Function::env(fn))) {
    std::vector<Frame*> context{};

    Cons::cons_iter<Type::Tag> iter(Function::env(fn));
    for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
      auto fn = it->car;
      auto frame = fp->env->MapFrame(Function::frame_id(fn));
      auto args = new Type::Tag[frame->nargs];

      for (size_t i = 0; i < frame->nargs; ++i) args[i] = frame->argv[i];

      auto nf =
          new Frame(fp->env, Function::frame_id(fn), fn, args, frame->nargs);
      context.push_back(nf);
    }

    Function::context(fn, context);
  }

  fp->value = fn;
}

/** * (.frame-ref fixnum fixnum) => object **/
auto FrameRef(Frame* fp) -> void {
  auto frame_id = fp->argv[0];
  auto offset = fp->argv[1];

  if (!Fixnum::IsType(frame_id))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     ".frame-ref", frame_id);

  if (!Fixnum::IsType(offset))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     ".frame-ref", offset);

  auto lfp = fp->env->MapFrame(frame_id);

  fp->value = lfp->argv[Fixnum::Uint64Of(offset)];
}

/** * (letq frame-id offset value) => value **/
auto Letq(Frame* fp) -> void {
  auto frame_id = fp->argv[0];
  auto offset = fp->argv[1];
  auto value = fp->argv[2];

  if (!Fixnum::IsType(frame_id))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "letq",
                     frame_id);

  if (!Fixnum::IsType(offset))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "letq",
                     offset);

  auto lfp = fp->env->MapFrame(frame_id);

  lfp->argv[Fixnum::Uint64Of(offset)] = value;

  fp->value = value;
}

} /* namespace mu */
} /* namespace libmu */
