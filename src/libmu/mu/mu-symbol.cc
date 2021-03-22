/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 ** mu-symbol.cc: library symbol functions
 **
 **/
#include <cassert>

#include "libmu/core.h"
#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/types/condition.h"
#include "libmu/types/cons.h"
#include "libmu/types/function.h"
#include "libmu/types/string.h"
#include "libmu/types/symbol.h"

namespace libmu {
namespace mu {

using Condition = core::Condition;
using String = core::String;
using Symbol = core::Symbol;
using Frame = core::Env::Frame;
using Type = core::Type;

/** * (symbol-value symbol) => object **/
auto SymbolValue(Frame* fp) -> void {
  auto symbol = fp->argv[0];

  if (!Symbol::IsType(symbol))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "symbol-value", symbol);

  if (!Symbol::IsBound(symbol))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::UNBOUND_VARIABLE,
                     "symbol-value", symbol);

  fp->value = Symbol::value(symbol);
}

/** * (symbol-name symbol) => string **/
auto SymbolName(Frame* fp) -> void {
  auto symbol = fp->argv[0];

  if (!Symbol::IsType(symbol))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "name-of",
                     symbol);

  fp->value = Symbol::name(symbol);
}

/** * (symbol-namespace symbol) => namespace **/
auto SymbolNamespace(Frame* fp) -> void {
  auto symbol = fp->argv[0];

  if (!Symbol::IsType(symbol))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "symbol-namespace", symbol);

  fp->value = Symbol::ns(symbol);
}

/** * (symbolp object) => bool **/
auto IsSymbol(Frame* fp) -> void {
  fp->value = Type::Bool(Symbol::IsType(fp->argv[0]));
}

/** * (keyword? object) => bool **/
auto IsKeyword(Frame* fp) -> void {
  fp->value = Type::Bool(Symbol::IsKeyword(fp->argv[0]));
}

/** * (bound? symbol) => bool **/
auto IsBound(Frame* fp) -> auto {
  fp->value = Type::Bool(Symbol::IsBound(fp->argv[0]));
}

/** * (uninterned-symbol string) => symbol **/
auto UninternedSymbol(Frame* fp) -> void {
  if (!String::IsType(fp->argv[0]))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "uninterned-symbol", fp->argv[0]);

  fp->value = Symbol(Type::NIL, fp->argv[0]).Evict(fp->env);
}

/** * (keyword string) */
auto MakeKeyword(Frame* fp) -> void {
  if (!String::IsType(fp->argv[0]))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "make-keyword", fp->argv[0]);

  fp->value = Symbol::Keyword(fp->argv[0]);
}

} /* namespace mu */
} /* namespace libmu */
