/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  macro.cc: library macros
 **
 **/
#include "libmu/macro.h"

#include <cassert>
#include <functional>
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
namespace core {
namespace {

/** * expand macro once **/
auto MacroExpand1(Env* env, Tag form) -> std::pair<bool, Tag> {
  auto not_expanded = std::pair<bool, Tag>{false, form};

  if (!Cons::IsType(form)) return not_expanded;

  auto fn = Cons::car(form);
  if (!Symbol::IsType(fn)) return not_expanded;

  auto macfn = Macro::MacroFunction(env, fn);
  if (Type::Null(macfn)) return not_expanded;

  std::vector<Tag> argv{};
  Cons::ListToVec(Cons::cdr(form), argv);

  return std::pair<bool, Tag>{true, Function::Funcall(env, macfn, argv)};
}

} /* anonymous namespace */

/** * garbage collection **/
auto Macro::GcMark(Env* env, Tag macro) -> void {
  assert(IsType(macro));

  env->heap_->GcMark(macro);
  env->GcMark(env, func(macro));
}

/** * make view of macro **/
auto Macro::ViewOf(Env* env, Tag macro) -> Tag {
  assert(IsType(macro));

  auto fn = func(macro);

  /* think: need nreqs? do we have any core macro expanders? */
  auto view = std::vector<Tag>{Symbol::Keyword("macro"),
                               Fixnum(ToUint64(macro) >> 3).tag_,
                               macro,
                               fn,
                               Function::mu(fn),
                               Function::form(fn),
                               Function::frame_id(fn),
                               Fixnum(Function::arity(fn)).tag_,
                               Function::name(fn)};

  return Vector(env, view).tag_;
}

/** * macro printer **/
auto Macro::Print(Env* env, Tag macro, Tag str, bool) -> void {
  auto fn = func(macro);

  assert(Function::IsType(fn));
  assert(Stream::IsType(str));

  auto stream = Stream::StreamDesignator(env, str);
  auto name = String::StdStringOf(Symbol::name(Function::name(fn)));

  std::stringstream hexs;

  hexs << std::hex << Type::to_underlying(fn);
  core::PrintStdString(env, "#<:macro #x" + hexs.str() + " (" + name + ")>",
                       stream, false);
}

/** * macro-function accessore **/
auto Macro::MacroFunction(Env* env, Tag macsym) -> Tag {
  assert(Symbol::IsType(macsym));

  auto macfn = Symbol::value(core::Compile(env, macsym));

  return IsType(macfn) ? func(macfn) : NIL;
}

/** * expand macro call until it isn't **/
auto Macro::MacroExpand(Env* env, Tag form) -> Tag {
  auto expanded = form;
  bool expandp;

  do std::tie<bool, Tag>(expandp, expanded) = MacroExpand1(env, expanded);
  while (expandp);

  return expanded;
}

} /* namespace core */
} /* namespace libmu */
