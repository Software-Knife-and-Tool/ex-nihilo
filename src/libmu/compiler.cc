/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  compile.cc: library form compiler
 **
 **/
#include "libmu/compiler.h"

#include <cassert>
#include <functional>
#include <map>

#include "libmu/env.h"
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

/** * compile form **/
TagPtr Compiler::Compile(Env* env, TagPtr form) {
  auto rval = Type::NIL;

  switch (Type::TypeOf(form)) {
    case SYS_CLASS::CONS: { /* funcall/macro call/special call */

      auto fn = Cons::car(form);
      auto args = Cons::cdr(form);

      switch (Type::TypeOf(fn)) {
        case SYS_CLASS::CONS: { /* lambda form */
          if (Type::Eq(Cons::car(fn), Symbol::Keyword("lambda"))) {
            auto lambda = Cons::Nth(fn, 1);

            if (!Cons::IsList(lambda))
              Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                               "lambda list", lambda);

            /* compile (lambda-func . body) */
            rval =
              CompileList(env,
                          Cons(CompileLambda(env, lambda, Cons::NthCdr(fn, 2)),
                               args).Evict(env, "compile:lambda"));
          } else {
            rval = CompileList(env, form);
          }
          break;
        }
        case SYS_CLASS::NULLT: /* fall through */
        case SYS_CLASS::SYMBOL: { /* funcall/macro call/special call */
          auto lfn = Type::NIL;
          size_t nth;

          rval = form;
          if (InLexicalEnv(env, fn, &lfn, &nth)) { /* lexical function */
            std::vector<TagPtr> lex{
              Namespace::InternInNs(env, env->mu_, String(env, "apply").tag_),
              Compile(env, CompileLexical(env, lfn, nth)),
              Cons::List(env,
                         std::vector<TagPtr>{
                           env->map_eval_,
                           Cons::List(env, std::vector<TagPtr>{
                               Symbol::Keyword("quote"),
                               CompileList(env, args)})})};

            rval = Compile(env, Cons::List(env, lex));
          } else if (!Type::Null(Macro::MacroFunction(env, fn))) {
            rval = Compile(env, Macro::MacroExpand(env, form));
          } else if (IsSpecOp(env, fn)) {
            rval = CompileSpecial(env, form);
          } else {
            rval = CompileList(env, form);
          }
          break;
        }
        case SYS_CLASS::FUNCTION: /* function */
          rval = CompileList(env, form);
          break;
        default:
          Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                           "compile function type", fn);
          break;
      }
      break;
    }
    case SYS_CLASS::SYMBOL: {
      auto fn = Type::NIL;
      size_t nth;

      rval =
        (InLexicalEnv(env, form, &fn, &nth)) ?
            Compile(env, CompileLexical(env, fn, nth)) :
            form;
      break;
    }
    default: /* constant */
      rval = form;
      break;
  }

  return rval;
}

/** * compile list of forms **/
Type::TagPtr Compiler::CompileList(Env* env, TagPtr list) {
  assert(Cons::IsList(list));

  return Type::Null(list)
             ? Type::NIL
             : Cons::MapCar(env,
                            [](Env* env, TagPtr form) {
                              return Compile(env, form);
                            },
                            list);
}

} /* namespace libmu */
