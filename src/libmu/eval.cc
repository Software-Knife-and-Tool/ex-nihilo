/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
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
namespace core {

/** * apply function to argument list **/
TagPtr Apply(Env* env, TagPtr fn, TagPtr args) {
  assert(Function::IsType(fn));
  assert(Cons::IsList(args));

  std::vector<TagPtr> argv;
  Cons::ListToVec(args, argv);

  return Function::Funcall(env, fn, argv);
}

/** * evaluate form in environment **/
TagPtr Eval(Env* env, TagPtr form) {
  TagPtr rval;

  switch (Type::TypeOf(form)) {
    case SYS_CLASS::SYMBOL:
      if (!Symbol::IsBound(form))
        Exception::Raise(env, Exception::EXCEPT_CLASS::UNBOUND_VARIABLE,
                         "(eval)", form);
      rval = Symbol::value(form);
      break;
    case SYS_CLASS::CONS: { /* function call */
      auto fn = Eval(env, Cons::car(form));

      switch (Type::TypeOf(fn)) { /* keyword, should be :quote, :t, or :nil */
        case SYS_CLASS::SYMBOL:
          if (Type::Eq(fn, Symbol::Keyword("quote")))
            rval = Cons::Nth(form, 1);
          else if (Type::Eq(fn, Symbol::Keyword("t")))
            rval = Eval(env, Cons::Nth(form, 1));
          else if (Type::Eq(fn, Symbol::Keyword("nil")))
            rval = Eval(env, Cons::Nth(form, 2));
          else
            Exception::Raise(env, Exception::EXCEPT_CLASS::UNDEFINED_FUNCTION,
                             "(eval)", fn);
          break;
        case SYS_CLASS::FUNCTION: { /* function object */
          std::vector<TagPtr> vlist;
          Cons::cons_iter<TagPtr> iter(Cons::cdr(form));
          for (auto it = iter.begin(); it != iter.end(); it = ++iter)
            vlist.push_back({Eval(env, it->car)});
          rval = Apply(env, fn, Cons::List(env, vlist));
          break;
        }
        default:
          Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(eval)",
                           fn);
      }
      break;
    }
    default: /* constant */
      rval = form;
      break;
  }

  return rval;
}

} /* namespace core */
} /* namespace libmu */
