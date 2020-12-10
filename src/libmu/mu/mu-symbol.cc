/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 ** mu-symbol.cc: library symbol functions
 **
 **/
#include <cassert>

#include "libmu/env.h"
#include "libmu/print.h"
#include "libmu/type.h"

#include "libmu/types/cons.h"
#include "libmu/types/exception.h"
#include "libmu/types/function.h"
#include "libmu/types/string.h"
#include "libmu/types/symbol.h"

namespace libmu {
namespace mu {

using Frame = Env::Frame;

/** * (symbol-value symbol) => object **/
void SymbolValue(Frame* fp) {
  auto symbol = fp->argv[0];

  if (!Symbol::IsType(symbol))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "symbol-value", symbol);

  if (!Symbol::IsBound(symbol))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::UNBOUND_VARIABLE,
                     "symbol-value", symbol);

  fp->value = Symbol::value(symbol);
}

/** * (symbol-name symbol) => string **/
void SymbolName(Frame* fp) {
  auto symbol = fp->argv[0];

  if (!Symbol::IsType(symbol))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "name-of",
                     symbol);

  fp->value = Symbol::name(symbol);
}

/** * (symbol-namespace symbol) => namespace **/
void SymbolNamespace(Frame* fp) {
  auto symbol = fp->argv[0];

  if (!Symbol::IsType(symbol))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "symbol-namespace", symbol);

  fp->value = Symbol::ns(symbol);
}

/** * (symbol? object) => bool **/
void IsSymbol(Frame* fp) {
  fp->value = Type::Bool(Symbol::IsType(fp->argv[0]));
}

/** * (keyword? object) => bool **/
void IsKeyword(Frame* fp) {
  fp->value = Type::Bool(Symbol::IsKeyword(fp->argv[0]));
}

/** * (bound? symbol) => bool **/
void IsBound(Frame* fp) {
  fp->value = Type::Bool(Symbol::IsBound(fp->argv[0]));
}

/** * (uninterned-symbol string) => symbol **/
void UninternedSymbol(Frame* fp) {
  if (!String::IsType(fp->argv[0]))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "uninterned-symbol", fp->argv[0]);

  fp->value =
      Symbol(Type::NIL, fp->argv[0]).Evict(fp->env, "mu-symbol:unintern");
}

/** * (keyword string) */
void MakeKeyword(Frame* fp) {
  if (!String::IsType(fp->argv[0]))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "make-keyword", fp->argv[0]);

  fp->value = Symbol::Keyword(fp->argv[0]);
}

} /* namespace mu */
} /* namespace libmu */
