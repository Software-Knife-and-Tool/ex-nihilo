/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu-type.cc: library type functions
 **
 **/
#include <cassert>
#include <functional>
#include <iomanip>
#include <map>
#include <vector>

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

/** * (type-of object) => symbol **/
void TypeOff(Frame* fp) {
  auto obj = fp->argv[0];

  fp->value = Struct::IsType(obj) ? Struct::stype(obj)
                                  : Type::MapClassSymbol(Type::TypeOf(obj));
}

/** * (eq object object) => bool **/
void Eq(Frame* fp) {
  fp->value = Type::BoolOf(Type::Eq(fp->argv[0], fp->argv[1]));
}

/** * (special-operator? form) => bool **/
void IsSpecOp(Frame* fp) {
  auto symbol = fp->argv[0];

  if (!Symbol::IsType(symbol))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "special-operator?", symbol);

  fp->value = Type::BoolOf(Compiler::IsSpecOp(fp->env, symbol));
}

/** * (make-view object) => vector **/
void MakeView(Frame* fp) { fp->value = Env::ViewOf(fp->env, fp->argv[0]); }

} /* namespace mu */
} /* namespace libmu */
