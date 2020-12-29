/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  function.cc: library functions
 **
 **/
#include "libmu/types/function.h"

#include <cassert>
#include <sstream>

#include "libmu/env.h"
#include "libmu/eval.h"
#include "libmu/print.h"
#include "libmu/type.h"

#include "libmu/compiler.h"

#include "libmu/types/cons.h"
#include "libmu/types/exception.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/symbol.h"
#include "libmu/types/vector.h"

namespace libmu {
namespace core {

/** * garbage collection **/
void Function::GcMark(Env* ev, TagPtr fn) {
  assert(IsType(fn));

  if (!ev->heap_->IsGcMarked(fn)) {
    ev->heap_->GcMark(fn);
    ev->GcMark(ev, env(fn));
    ev->GcMark(ev, form(fn));
    ev->GcMark(ev, name(fn));
    for (auto fp : context(fn)) {
      Env::GcFrame(fp);
    }
  }
}

/** * run-time function call argument arity validation **/
void Function::CheckArity(Env* env, TagPtr fn,
                          const std::vector<TagPtr>& args) {
  assert(IsType(fn));

  size_t nreqs = arity_nreqs(fn);
  auto rest = arity_rest(fn);
  auto nargs = args.size();

  if (nargs < nreqs)
    Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "argument list arity: nargs (" + std::to_string(nargs) +
                         ") < nreqs (" + std::to_string(nreqs) + ") (funcall)",
                     fn);

  if (!rest && (nargs > nreqs))
    Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "argument list arity: !rest && nargs (" +
                         std::to_string(nargs) + ") > nreq (" +
                         std::to_string(nreqs) + ") (funcall)",
                     fn);
}

/** * make view of function **/
TagPtr Function::ViewOf(Env* env, TagPtr fn) {
  assert(IsType(fn));

  /* think: add context */
  auto view = std::vector<TagPtr>{Symbol::Keyword("func"),
                                  fn,
                                  Fixnum(ToUint64(fn) >> 3).tag_,
                                  name(fn),
                                  core(fn),
                                  form(fn),
                                  frame_id(fn),
                                  Fixnum(arity(fn)).tag_};

  return Vector(env, view).tag_;
}

/** * call function with argument vector **/
TagPtr Function::Funcall(Env* env, TagPtr fn, const std::vector<TagPtr>& argv) {
  assert(IsType(fn));

  size_t nargs = arity_nreqs(fn) + ((arity_rest(fn) ? 1 : 0));

  CheckArity(env, fn, argv);

  auto args = std::make_unique<TagPtr[]>(nargs);
  if (nargs) {
    size_t i = 0;
    for (; i < arity_nreqs(fn); i++) args[i] = argv[i];

    if (arity_rest(fn)) {
      if (i == argv.size()) {
        args[i] = NIL;
      } else {
        std::vector<TagPtr> restv;

        for (size_t j = i; j < argv.size(); j++) restv.push_back(argv[j]);

        args[i] = Cons::List(env, restv);
      }
    }
  }

  Env::Frame fp(env, frame_id(fn), fn, args.get(), nargs);

  env->PushFrame(&fp);

  if (nargs) env->Cache(&fp);

  for (auto frame : Function::context(fn))
    if (frame->nargs) {
      env->Cache(frame);
    }

  CallFrame(&fp);

  for (auto frame : Function::context(fn))
    if (frame->nargs) {
      env->UnCache(frame);
    }

  if (nargs) env->UnCache(&fp);

  env->PopFrame();

  return fp.value;
}

} /* namespace core */
} /* namespace libmu */
