/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu-env.cc: library eval/apply/env
 **
 **/
#include <cassert>
#include <functional>
#include <iomanip>
#include <map>
#include <vector>

#include "libmu/compiler.h"
#include "libmu/core.h"
#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/types/condition.h"
#include "libmu/types/cons.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/float.h"
#include "libmu/types/function.h"
#include "libmu/types/stream.h"
#include "libmu/types/struct.h"
#include "libmu/types/symbol.h"

namespace libmu {
namespace mu {

using Condition = core::Condition;
using Frame = core::Env::Frame;
using Type = core::Type;

/** * (eval object) => object **/
auto Eval(Frame* fp) -> void {
  fp->value = core::Eval(fp->env, core::Compile(fp->env, fp->argv[0]));
}

/** * (env-view) => vector **/
auto EnvView(Frame* fp) -> void {
  auto view =
      std::vector<core::Tag>{core::Symbol::Keyword("env"), fp->env->namespace_,
                             core::Env::Namespaces(fp->env),
                             core::Env::EnvStack(fp->env), fp->env->src_form_};

  fp->value = core::Vector<Type::Tag>(view).tag_;
}

/** * (clock-view) => vector **/
auto ClockView(Frame* fp) -> void {
  uint64_t st, rt;

  core::Platform::SystemTime(&st);
  core::Platform::ProcessTime(&rt);

  auto view =
      std::vector<core::Tag>{core::Symbol::Keyword("clock"),
                             core::Fixnum(st).tag_, core::Fixnum(rt).tag_};

  fp->value = core::Vector<Type::Tag>(view).tag_;
}

/** * (apply func list) => object **/
auto Apply(Frame* fp) -> void {
  auto func = fp->argv[0];
  auto args = fp->argv[1];

  if (!(core::Function::IsType(func)))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "is not a function (.apply)", func);

  if (!(core::Cons::IsList(args)))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "is not a list (.apply)", args);

  std::vector<Type::Tag> argv;
  core::Cons::ListToVec(args, argv);

  fp->value = core::Function::Funcall(fp->env, func, argv);
}

} /* namespace mu */
} /* namespace libmu */
