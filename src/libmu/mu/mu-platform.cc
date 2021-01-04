/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
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

#include "libmu/env.h"
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
using Fixnum = core::Fixnum;
using Frame = core::Env::Frame;
using Platform = core::Platform;
using String = core::String;
using Type = core::Type;

/** *  (clocks fixnum) => (systime-msec . runtime-usec)  **/
void Clocks(Frame* fp) {
  uint64_t st, rt;

  Platform::SystemTime(&st);
  Platform::ProcessTime(&rt);

  fp->value = core::Cons(Fixnum(st).tag_, Fixnum(rt).tag_).Evict(fp->env);
}

/** *  (exit fixnum) never returns **/
void Exit(Frame* fp) {
  auto rc = fp->argv[0];

  if (!Fixnum::IsType(rc))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "exit", rc);

  std::exit(Fixnum::Int64Of(rc));
}

/** * (system string) **/
void System(Frame* fp) {
  auto cmd = fp->argv[0];

  if (!String::IsType(cmd))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "is not a string (.system)", cmd);

  fp->value = Fixnum(Platform::System(String::StdStringOf(cmd))).tag_;
}

/** * (.invoke fixnum string) => string **/
void Invoke(Frame* fp) {
  auto fn = fp->argv[0];
  auto arg = fp->argv[1];

  if (!Fixnum::IsType(fn))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, ".invoke",
                     fn);

  if (!String::IsType(arg))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, ".invoke",
                     arg);

  fp->value = String(fp->env, Platform::Invoke(Fixnum::Uint64Of(fn),
                                               String::StdStringOf(arg)))
                  .tag_;
}

} /* namespace mu */
} /* namespace libmu */
