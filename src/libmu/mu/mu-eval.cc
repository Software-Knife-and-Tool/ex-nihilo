/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu-eval.cc: library eval/apply
 **
 **/
#include <cassert>
#include <functional>
#include <iomanip>
#include <map>
#include <vector>

#include "libmu/compiler.h"
#include "libmu/env.h"
#include "libmu/eval.h"
#include "libmu/print.h"
#include "libmu/read.h"
#include "libmu/type.h"

#include "libmu/types/cons.h"
#include "libmu/types/exception.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/float.h"
#include "libmu/types/function.h"
#include "libmu/types/macro.h"
#include "libmu/types/stream.h"
#include "libmu/types/struct.h"
#include "libmu/types/symbol.h"

namespace libmu {
namespace mu {

using Frame = Env::Frame;

/** * (eval object) => object **/
void Eval(Frame* fp) {
  fp->value = core::Eval(fp->env, core::Compile(fp->env, fp->argv[0]));
}

/** * (%env-stack-depth object) => fixnum **/
void EnvStackDepth(Frame* fp) {
  fp->value = Fixnum(fp->env->frames_.size() - 1).tag_;
}

/** * (%env-stack) => list **/
void EnvStack(Frame* fp) {
  auto offset = fp->argv[0];
  auto take = fp->argv[1];

  if (!(Fixnum::IsType(offset)))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "is not a fixnum (env-stack)", offset);

  if (!(Fixnum::IsType(take)))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "is not a fixnum (env-stack)", take);

  std::vector<TagPtr> stack;

  auto n = Fixnum::Int64Of(take);

  for (auto fp : fp->env->frames_) {
    std::vector<TagPtr> frame;
    std::vector<TagPtr> args;

    if (n-- == 0) break;

    frame.push_back(Symbol::Keyword("frame"));
    frame.push_back(fp->func);
    frame.push_back(Fixnum(fp->nargs).tag_);

    for (size_t i = 0; i < fp->nargs; ++i) args.push_back(fp->argv[i]);

    frame.push_back(Cons::List(fp->env, args));
    frame.push_back(fp->frame_id);

    stack.push_back(Vector(fp->env, frame).tag_);
  }

  fp->value = Cons::List(fp->env, stack);
}

/** * mu function (apply func list) => object **/
void Apply(Frame* fp) {
  auto func = fp->argv[0];
  auto args = fp->argv[1];

  if (!(Function::IsType(func)))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "is not a function (.apply)", func);

  if (!(Cons::IsList(args)))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "is not a list (.apply)", args);

  std::vector<TagPtr> argv;
  Cons::MapC(
      fp->env, [&argv](Env*, Type::TagPtr form) { argv.push_back(form); },
      fp->argv[1]);

  fp->value = Function::Funcall(fp->env, func, argv);
}

} /* namespace mu */
} /* namespace libmu */
