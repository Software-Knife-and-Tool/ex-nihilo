/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  special.cc: library special forms
 **
 **/
#include <cassert>
#include <functional>
#include <map>
#include <vector>

#include "libmu/compiler.h"
#include "libmu/env.h"
#include "libmu/eval.h"
#include "libmu/print.h"
#include "libmu/print.h"
#include "libmu/type.h"

#include "libmu/types/cons.h"
#include "libmu/types/exception.h"
#include "libmu/types/function.h"
#include "libmu/types/macro.h"
#include "libmu/types/namespace.h"
#include "libmu/types/string.h"
#include "libmu/types/symbol.h"

namespace libmu {
namespace {

/** * map keyword to handler **/
static const std::map<TagPtr, std::function<TagPtr(Env*, TagPtr)>> kSpecMap{
    {Symbol::Keyword("defsym"), Compiler::DefConstant},
    {Symbol::Keyword("lambda"), Compiler::Lambda},
    {Symbol::Keyword("letq"), Compiler::Letq},
    {Symbol::Keyword("macro"), Compiler::DefMacro},
    {Symbol::Keyword("quote"), Compiler::Quote}};

} /* anonymous namespace */

/** * special operator predicate **/
bool Compiler::IsSpecOp(Env* env, TagPtr symbol) {
  if (!Symbol::IsType(symbol))
    Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "special-operatorp: is not a symbol", symbol);

  return Symbol::IsKeyword(symbol) && (kSpecMap.count(symbol) != 0);
}

/** * compile a special form **/
Type::TagPtr Compiler::CompileSpecial(Env* env, TagPtr form) {
  assert(Cons::IsList(form));

  auto symbol = Cons::car(form);
  assert(IsSpecOp(env, symbol));

  return kSpecMap.at(symbol)(env, form);
}

/** * (:defcon symbol form) **/
Type::TagPtr Compiler::DefConstant(Env* env, TagPtr form) {
  if (Cons::Length(env, form) != 3)
    Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     ":defsym: argument count(2)", form);

  auto args = Cons::cdr(form);
  auto sym = Cons::Nth(args, 0);
  auto expr = Cons::Nth(args, 1);

  if (!Symbol::IsType(sym))
    Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "is not a symbol (:defsym)", sym);

  if (Symbol::IsKeyword(sym))
    Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "can't bind keywords (:defsym)", sym);

  if (Symbol::IsBound(sym))
    Exception::Raise(env, Exception::EXCEPT_CLASS::CELL_ERROR,
                     "symbol previously bound (:defsym)", sym);

  auto value = Eval(env, Compile(env, expr));
  (void)Symbol::Bind(sym, value);

  if (Function::IsType(value))
    Function::name(value, sym);
  
  return Cons::List(env, std::vector<TagPtr>{Symbol::Keyword("quote"), sym});
}

/** * (:lambda list . body) **/
Type::TagPtr Compiler::Lambda(Env* env, TagPtr form) {
  if (Cons::Length(env, form) == 1)
    Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     ":lambda: argument count(1*)", form);

  auto args = Cons::cdr(form);
  auto lambda = Cons::Nth(args, 0);

  if (!Cons::IsList(lambda))
    Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR, ":lambda", lambda);

  assert(!Type::Eq(Cons::car(lambda), Symbol::Keyword("quote")));

  return CompileLambda(env, form);
}

/** * (:macro list . body) **/
Type::TagPtr Compiler::DefMacro(Env* env, TagPtr form) {
  if (Cons::Length(env, form) == 1)
    Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     ":macro: argument count(1*)", form);

  auto args = Cons::cdr(form);
  auto lambda = Cons::Nth(args, 0);

  if (!Cons::IsList(lambda))
    Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR, ":macro", lambda);

  assert(!Type::Eq(Cons::car(lambda), Symbol::Keyword("quote")));

  return Macro(CompileLambda(env, form)).Evict(env, "macro");
}

/** * (:letq symbol expr) **/
Type::TagPtr Compiler::Letq(Env* env, TagPtr form) {
  if (Cons::Length(env, form) != 3)
    Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     ":letq: argument count(2)", form);

  auto args = Cons::cdr(form);
  auto sym = Cons::Nth(args, 0);
  auto expr = Cons::Nth(args, 1);

  if (!Symbol::IsType(sym))
    Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR, ":letq", sym);

  auto lsym = Compiler::Compile(env, sym);

  if (!Cons::IsList(lsym))
    Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR, ":letq", lsym);

  auto letq = Namespace::FindInNsInterns(env, env->mu_, String(env, "letq").tag_);
  assert(!Null(letq));
  
  return Cons::List(env,
                    std::vector<TagPtr>{letq,
                                        Cons::Nth(lsym, 1),
                                        Cons::Nth(lsym, 2),
                                        Compiler::Compile(env, expr)});
}

/** * (:quote object) **/
Type::TagPtr Compiler::Quote(Env* env, TagPtr form) {
  if (Cons::Length(env, form) != 2)
    Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     ":quote: argument count(1)", form);

  auto args = Cons::cdr(form);
  auto quoted = Cons::Nth(args, 1);

  return Cons::List(env, std::vector<TagPtr>{Symbol::Keyword("quote"), quoted});
}

} /* namespace libmu */

