/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu-platform.cc: platform functions
 **
 **/
#include <cassert>
#include <functional>
#include <iomanip>
#include <map>
#include <vector>

#include "libmu/platform/platform.h"

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
using Fixnum = core::Fixnum;
using Frame = core::Env::Frame;
using Platform = core::Platform;
using String = core::String;
using Type = core::Type;

/** *  (exit fixnum) never returns **/
[[noreturn]] auto Exit(Frame* fp) -> void {
  auto rc = fp->argv[0];

  if (!Fixnum::IsType(rc))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "exit",
                     rc);

  std::exit(Fixnum::Int64Of(rc));
}

/** * (system string) **/
auto System(Frame* fp) -> void {
  auto cmd = fp->argv[0];

  if (!String::IsType(cmd))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "is not a string (.system)", cmd);

  fp->value = Fixnum(Platform::System(String::StdStringOf(cmd))).tag_;
}

/** * (invoke fixnum string) => string **/
auto Invoke(Frame* fp) -> void {
  auto fn = fp->argv[0];
  auto arg = fp->argv[1];

  if (!Fixnum::IsType(fn))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, ".invoke",
                     fn);

  if (!String::IsType(arg))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, ".invoke",
                     arg);

  fp->value =
      String(Platform::Invoke(Fixnum::Uint64Of(fn), String::StdStringOf(arg)))
          .tag_;
}

} /* namespace mu */
} /* namespace libmu */
