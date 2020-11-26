/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  eval.cc: library eval/apply
 **
 **/
#include "libmu/eval.h"

#include <cassert>
#include <functional>
#include <iomanip>
#include <map>
#include <vector>

#include "libmu/env.h"
#include "libmu/print.h"
#include "libmu/read.h"
#include "libmu/type.h"

#include "libmu/compiler.h"

#include "libmu/types/cons.h"
#include "libmu/types/exception.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/float.h"
#include "libmu/types/function.h"
#include "libmu/types/macro.h"
#include "libmu/types/namespace.h"
#include "libmu/types/stream.h"
#include "libmu/types/struct.h"
#include "libmu/types/symbol.h"
#include "libmu/types/vector.h"

namespace libmu {

using TagPtr = Type::TagPtr;
using SYS_CLASS = Type::SYS_CLASS;

/** * apply eval'd argument list to function **/
Type::TagPtr Apply(Env* env, TagPtr fn, TagPtr form) {
  assert(Function::IsType(fn));
  assert(Cons::IsList(form));

  std::vector<TagPtr> argv;
  Cons::MapC(
      env, [&argv](Env* env, TagPtr form) { argv.push_back(Eval(env, form)); },
      Cons::cdr(form));

  return Function::Funcall(env, fn, argv);
}

/** * evaluate form in environment **/
Type::TagPtr Eval(Env* env, TagPtr form) {
  auto rval = form;

  switch (Type::TypeOf(form)) {
    case SYS_CLASS::CONS: { /* function call */
      auto fn = Eval(env, Cons::car(form));

      switch (Type::TypeOf(fn)) {
        case SYS_CLASS::SYMBOL: /* (symbol ...) */
          assert(!(Compiler::IsSpecOp(env, fn) &&
                   !Type::Eq(fn, Symbol::Keyword("quote"))));
          assert(Type::Null(Macro::MacroFunction(env, fn)));

          if (Type::Eq(fn, Symbol::Keyword("quote"))) { /* quoted form */
            rval = Cons::Nth(form, 1);
          } else if (Function::IsType(Symbol::value(fn))) { /* function call */
            rval = Apply(env, Symbol::value(fn), form);
          } else
            Exception::Raise(env, Exception::EXCEPT_CLASS::UNDEFINED_FUNCTION,
                             "(eval)", fn);
          break;
        case SYS_CLASS::FUNCTION: /* function object */
          rval = Apply(env, fn, form);
          break;
        default:
          Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(eval)", fn);
      }

      break;
    }
    case SYS_CLASS::SYMBOL:
      if (!Symbol::IsBound(form))
        Exception::Raise(env, Exception::EXCEPT_CLASS::UNBOUND_VARIABLE, "(eval)",
                         form);
      rval = Symbol::value(form);
      break;
    default: /* constant form */
      rval = form;
      break;
  }

  return rval;
}

} /* namespace libmu */
