/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
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

#include "platform/platform.h"

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

/** *  (stack-trace) => :nil **/
void StackTrace(Frame* fp) {
  // stackTrace(fp->env);

  fp->value = Type::NIL;
}

/** *  (stack-gc) => :nil **/
void StackInspect(Frame* fp) {
  // stackInspect(fp->env);

  fp->value = Type::NIL;
}

/** *  (systime => list **/
void SystemTime(Frame* fp) {
  unsigned long ts[2];

  Platform::SystemTime(ts);

  fp->value = Cons::List(
      fp->env, std::vector<TagPtr>{Fixnum(ts[0]).tag_, Fixnum(ts[1]).tag_});
}

/** *  (runtime) => list **/
void RunTime(Frame* fp) {
  unsigned long ts[2];

  Platform::ProcessTime(ts);

  fp->value = Cons::List(
      fp->env, std::vector<TagPtr>{Fixnum(ts[0]).tag_, Fixnum(ts[1]).tag_});
}

/** *  (exit fixnum) never returns **/
void Exit(Frame* fp) {
  auto rc = fp->argv[0];

  if (!Fixnum::IsType(rc))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "exit", rc);

  std::exit(Fixnum::Int64Of(rc));
}

/** *  (system string) **/
void System(Frame* fp) {
  auto cmd = fp->argv[0];

  if (!String::IsType(cmd))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "is not a string (system)", cmd);

  fp->value = Fixnum(Platform::System(String::StdStringOf(cmd))).tag_;
}

/** * (system-env string) **/
void SystemEnv(Frame* fp) {
  auto env = Platform::Environment();
  auto vars = std::vector<TagPtr>{};

  for (auto ep = env; *ep != NULL; ep++) {
    auto env_var = std::string(*ep);
    auto eq = strchr(*ep, '=');

    assert(eq != NULL);

    auto name = String(fp->env, env_var.substr(0, eq - *ep)).tag_;
    auto value = String(fp->env, env_var.substr(eq - *ep + 1)).tag_;

    vars.push_back(Cons(name, value).Evict(fp->env, "mu-platform:system-env"));
  }

  fp->value = Cons::List(fp->env, vars);
}

/** * (invoke fixnum string) => string **/
void Invoke(Frame* fp) {
  auto fn = fp->argv[0];
  auto arg = fp->argv[1];

  if (!Fixnum::IsType(fn))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "%invoke",
                     fn);

  if (!String::IsType(arg))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "%invoke",
                     arg);

  fp->value = String(fp->env, Platform::Invoke(Fixnum::Uint64Of(fn),
                                               String::StdStringOf(arg)))
                  .tag_;
}

} /* namespace mu */
} /* namespace libmu */
