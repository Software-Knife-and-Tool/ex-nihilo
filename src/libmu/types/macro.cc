/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  macro.cc: library macros
 **
 **/
#include "libmu/types/macro.h"

#include <cassert>
#include <functional>
#include <vector>

#include "libmu/compiler.h"
#include "libmu/env.h"
#include "libmu/print.h"
#include "libmu/type.h"

#include "libmu/types/cons.h"
#include "libmu/types/exception.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/float.h"
#include "libmu/types/function.h"
#include "libmu/types/stream.h"
#include "libmu/types/struct.h"
#include "libmu/types/symbol.h"

namespace libmu {
namespace core {
namespace {

auto MacroExpand1(Env* env, TagPtr form) {
  auto not_expanded = std::pair<bool, TagPtr>{false, form};

  if (!Cons::IsType(form)) return not_expanded;

  auto fn = Cons::car(form);
  if (!Symbol::IsType(fn)) return not_expanded;

  auto macfn = Macro::MacroFunction(env, fn);
  if (Type::Null(macfn)) return not_expanded;

  std::vector<TagPtr> argv{};
  Cons::ListToVec(Cons::cdr(form), argv);

  return std::pair<bool, TagPtr>{true, Function::Funcall(env, macfn, argv)};
}

} /* anonymous namespace */

/** * garbage collection **/
void Macro::GcMark(Env* env, TagPtr macro) {
  assert(IsType(macro));

  env->heap_->GcMark(macro);
  env->GcMark(env, func(macro));
}

/** * make view of macro **/
TagPtr Macro::ViewOf(Env* env, TagPtr macro) {
  assert(IsType(macro));

  auto fn = func(macro);

  /* think: need nreqs? do we have any core macro expanders? */
  auto view = std::vector<TagPtr>{Symbol::Keyword("macro"),
                                  Fixnum(ToUint64(macro) >> 3).tag_,
                                  macro,
                                  fn,
                                  Function::core(fn),
                                  Function::form(fn),
                                  Function::frame_id(fn),
                                  Fixnum(Function::arity(fn)).tag_,
                                  Function::name(fn)};

  return Vector(env, view).tag_;
}

/** * macro-function predicate **/
TagPtr Macro::MacroFunction(Env* env, TagPtr macsym) {
  assert(Symbol::IsType(macsym));

  auto macfn = Symbol::value(core::Compile(env, macsym));

  return IsType(macfn) ? func(macfn) : NIL;
}

/** * expand macro call until it isn't **/
TagPtr Macro::MacroExpand(Env* env, TagPtr form) {
  auto expanded = form;
  bool expandp;

  do std::tie<bool, TagPtr>(expandp, expanded) = MacroExpand1(env, expanded);
  while (expandp);

  return expanded;
}

} /* namespace core */
} /* namespace libmu */
