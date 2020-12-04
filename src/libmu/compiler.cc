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
        case SYS_CLASS::CONS: /* list form */
          rval = Compile(env,
                         Cons(Compile(env, fn),
                              Cons::cdr(form)).Evict(env, "compile:fn"));
          break;
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
      case SYS_CLASS::MACRO: /* macro call */
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

  return Cons::MapCar(env,
                      [](Env* env, TagPtr form) {
                        return Compile(env, form);
                      },
                      list);
}

/** * compile lexical symbol */
Type::TagPtr Compiler::CompileLexical(Env* env, TagPtr fn, size_t nth) {
  assert(Function::IsType(fn));

  return
    Cons::List(env,
               std::vector<TagPtr>{
                 Namespace::FindInNsInterns(env,
                                            env->mu_,
                                            String(env, "frame-ref").tag_),
                 Function::frame_id(fn), Fixnum(nth).tag_});
}

/** * parse lambda list **/
Type::TagPtr Compiler::ParseLambda(Env* env, TagPtr lambda) {
  assert(Cons::IsList(lambda));

  std::vector<TagPtr> lexicals;

  auto restsym = Type::NIL;
  auto has_rest = false;

  std::function<void(Env*, TagPtr)> parse =
    [&lexicals, lambda, &restsym, &has_rest](Env* env,
                                             TagPtr symbol) {
    if (!Symbol::IsType(symbol))
      Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                       "non-symbol in lambda list (parse-lambda)", lambda);

    if (Type::Eq(Symbol::Keyword("rest"), symbol)) {
      if (has_rest)
        Exception::Raise(env, Exception::EXCEPT_CLASS::PARSE_ERROR,
                         "multiple rest clauses (parse-lambda)", lambda);
      has_rest = true;
      return;
    }

    if (Symbol::IsKeyword(symbol))
      Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                       "keyword cannot be used as a lexical variable (parse-lambda)",
                       lambda);

    auto el = std::find_if(
        lexicals.begin(), lexicals.end(), [restsym, symbol](TagPtr sym) {
          return Type::Eq(symbol, sym) || Type::Eq(symbol, restsym);
        });

    if (el != lexicals.end())
      Exception::Raise(env, Exception::EXCEPT_CLASS::PARSE_ERROR,
                       "duplicate symbol in lambda list (parse-lambda)",
                       lambda);

    if (has_rest)
      restsym = symbol;
    
    lexicals.push_back(symbol);
  };

  Cons::MapC(env, parse, lambda);

  if (has_rest && Type::Null(restsym))
    Exception::Raise(env, Exception::EXCEPT_CLASS::PARSE_ERROR,
                     "early end of lambda list (parse-lambda)", lambda);

  if (lexicals.size() != (Cons::Length(env, lambda) - has_rest))
    Exception::Raise(env, Exception::EXCEPT_CLASS::PARSE_ERROR,
                     ":rest should terminate lambda list (parse-lambda)",
                     lambda);

  /** * ((lexicals...) . restsym) */
  return Cons(Cons::List(env, lexicals), restsym).Evict(env, "lambda:parse-lambda");
}
  
/** * compile function definition **/
Type::TagPtr Compiler::CompileLambda(Env* env, TagPtr form) {
  assert(Cons::IsList(form));

  auto lambda = Compiler::ParseLambda(env, Cons::car(form));

  auto fn =
    Function(env,
             NIL,
             std::vector<Frame*>{},
             lambda,
             Cons(lambda, NIL).tag_).Evict(env, "compiler::compile-lambda");

  if (Function::arity(fn))
    env->lexenv_.push_back(fn);

  /* think: this is ugly */
  Function::form(fn,
                 Cons(lambda,
                      CompileList(env,
                                  Cons::cdr(form))).Evict(env,
                                                          "compiler::compile-lambda"));

  if (Function::arity(fn))
    env->lexenv_.pop_back();

  return fn;
}

/** * is this symbol in the lexical environment? **/
bool Compiler::InLexicalEnv(Env* env,
                            TagPtr sym,
                            TagPtr* lambda,
                            size_t* nth) {

  assert(Symbol::IsType(sym) || Symbol::IsKeyword(sym));
    
  if (Symbol::IsKeyword(sym))
    return false;

  std::vector<TagPtr>::reverse_iterator it;
  for (it = env->lexenv_.rbegin(); it != env->lexenv_.rend(); ++it) {
    assert(Function::IsType(*it));

    auto lexicals = Compiler::lexicals(Cons::car(Function::form(*it)));
    assert(Cons::IsList(lexicals));

    for (size_t i = 0; i < Cons::Length(env, lexicals); ++i) {
      if (Eq(sym, Cons::Nth(lexicals, i))) {
        *lambda = *it;
        *nth = i;
        return true;
      }
    }
  }
  
  return false;
}

} /* namespace libmu */
