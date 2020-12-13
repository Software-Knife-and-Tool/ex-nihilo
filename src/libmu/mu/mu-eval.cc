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

using Exception = core::Exception;
using Frame = core::Env::Frame;
using Type = core::Type;

/** * (eval object) => object **/
void Eval(Frame* fp) {
  fp->value = core::Eval(fp->env, core::Compile(fp->env, fp->argv[0]));
}

/** * (.env) => vector **/
void EnvView(Frame* fp) { fp->value = core::Env::EnvView(fp->env); }

/** * (apply func list) => object **/
void Apply(Frame* fp) {
  auto func = fp->argv[0];
  auto args = fp->argv[1];

  if (!(core::Function::IsType(func)))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "is not a function (.apply)", func);

  if (!(core::Cons::IsList(args)))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "is not a list (.apply)", args);

  std::vector<Type::TagPtr> argv;
  core::Cons::ListToVec(args, argv);

  fp->value = core::Function::Funcall(fp->env, func, argv);
}

} /* namespace mu */
} /* namespace libmu */
