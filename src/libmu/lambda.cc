/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  lambda.cc: library lambda compiler
 **
 **/
#include "libmu/compiler.h"

#include <cassert>
#include <functional>
#include <map>

#include "libmu/env.h"
#include "libmu/print.h"
#include "libmu/type.h"

#include "libmu/types/macro.h"
#include "libmu/types/cons.h"
#include "libmu/types/exception.h"
#include "libmu/types/function.h"
#include "libmu/types/namespace.h"
#include "libmu/types/string.h"
#include "libmu/types/symbol.h"

namespace libmu {

/** * parse lambda list **/
Type::TagPtr Compiler::ParseLambda(Env* env, TagPtr lambda_list) {
  assert(Cons::IsList(lambda_list));

  std::vector<TagPtr> lexicals;

  auto restsym = Type::NIL;
  bool has_rest = false;

  std::function<void(Env*, TagPtr)> parse =
    [&lexicals, lambda_list, &restsym, &has_rest](Env* env,
                                                  TagPtr symbol) {
    if (!Symbol::IsType(symbol))
      Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                       "non-symbol in lambda list (parse-lambda)", lambda_list);

    if (Type::Eq(Symbol::Keyword("rest"), symbol)) {
      if (has_rest)
        Exception::Raise(env, Exception::EXCEPT_CLASS::PARSE_ERROR,
                         "multiple rest clauses (parse-lambda)", lambda_list);
      has_rest = true;
      return;
    }

    if (Symbol::IsKeyword(symbol))
      Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                       "keyword cannot be used as a lexical variable (parse-lambda)",
                       lambda_list);

    auto el = std::find_if(
        lexicals.begin(), lexicals.end(), [restsym, symbol](TagPtr sym) {
          return Type::Eq(symbol, sym) || Type::Eq(symbol, restsym);
        });

    if (el != lexicals.end())
      Exception::Raise(env, Exception::EXCEPT_CLASS::PARSE_ERROR,
                       "duplicate symbol in lambda list (parse-lambda)",
                       lambda_list);

    if (has_rest) restsym = symbol;

    lexicals.push_back(symbol);
  };

  Cons::MapC(env, parse, lambda_list);

  if (has_rest && Type::Null(restsym))
    Exception::Raise(env, Exception::EXCEPT_CLASS::PARSE_ERROR,
                     "early end of lambda list (parse-lambda)", lambda_list);

  if (lexicals.size() != (Cons::Length(env, lambda_list) - has_rest))
    Exception::Raise(env, Exception::EXCEPT_CLASS::PARSE_ERROR,
                     ":rest should terminate lambda list (parse-lambda)",
                     lambda_list);

  /** * ((lexicals...) . restsym) */
  return Cons(Cons::List(env, lexicals), restsym).Evict(env, "lambda:parse-lambda");
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

/** * create function **/
Type::TagPtr Compiler::CompileLambda(Env* env, TagPtr form) {
  assert(Cons::IsList(form));

  auto fn = Function(env, form, NIL).Evict(env, "lambda:compile-lambda");
  auto lambda = Cons::car(form);
  
  if (!Type::Null(lambda)) env->lexenv_.push_back(form);

  Function::form(fn, Cons(lambda, CompileList(env, Cons::cdr(form))).tag_);

  if (!Type::Null(lambda)) env->lexenv_.pop_back();

  return fn;
}

/** * is this symbol in the lexical environment? **/
bool Compiler::InLexicalEnv(Env* env,
                            TagPtr sym,
                            TagPtr* lambda,
                            size_t* nth) {
  assert(Symbol::IsType(sym));

  if (Symbol::IsKeyword(sym)) return false;

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
