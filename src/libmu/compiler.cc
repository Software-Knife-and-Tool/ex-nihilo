/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
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
#include <utility>

#include "libmu/env.h"
#include "libmu/macro.h"
#include "libmu/print.h"
#include "libmu/type.h"

#include "libmu/types/condition.h"
#include "libmu/types/cons.h"
#include "libmu/types/function.h"
#include "libmu/types/namespace.h"
#include "libmu/types/string.h"
#include "libmu/types/symbol.h"

namespace libmu {
namespace core {
namespace {

/** * compile a list of forms **/
auto List(Env* env, Tag list) {
  std::vector<Tag> vlist;
  Cons::cons_iter<Tag> iter(list);

  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    vlist.push_back(Compile(env, it->car));

  return Cons::List(env, vlist);
}

/** * is this symbol in the lexical environment? **/
auto LexicalEnv(Env* env, Tag sym) -> std::pair<Tag, size_t> {
  assert(Symbol::IsType(sym) || Symbol::IsKeyword(sym));

  auto not_found = std::pair<Tag, size_t>{env->nil_, 0};

  if (Symbol::IsKeyword(sym)) return not_found;

  std::vector<Tag>::reverse_iterator it;
  for (it = env->lexenv_.rbegin(); it != env->lexenv_.rend(); ++it) {
    assert(Function::IsType(*it));

    auto lexicals = core::lexicals(Cons::car(Function::form(*it)));
    assert(Cons::IsList(lexicals));

    for (size_t i = 0; i < Cons::Length(env, lexicals); ++i) {
      if (Type::Eq(sym, Cons::Nth(lexicals, i)))
        return std::pair<Tag, size_t>{*it, i};
    }
  }

  return not_found;
}

/** * compile lambda definition **/
auto Lambda(Env* env, Tag form) {
  assert(Cons::IsList(form));

  std::function<Tag(Env*, Tag)> parse_lambda = [](Env* env, Tag lambda) {
    std::vector<Tag> lexicals;

    auto restsym = Type::NIL;
    auto has_rest = false;

    std::function<void(Env*, Tag)> parse = [&lexicals, lambda, &restsym,
                                            &has_rest](Env* env, Tag symbol) {
      if (!Symbol::IsType(symbol))
        Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR,
                         "non-symbol in lambda list (parse-lambda)", lambda);

      if (Type::Eq(Symbol::Keyword("rest"), symbol)) {
        if (has_rest)
          Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                           "multiple rest clauses (parse-lambda)", lambda);
        has_rest = true;
        return;
      }

      if (Symbol::IsKeyword(symbol))
        Condition::Raise(
            env, Condition::CONDITION_CLASS::TYPE_ERROR,
            "keyword cannot be used as a lexical variable (parse-lambda)",
            lambda);

      auto el = std::find_if(
          lexicals.begin(), lexicals.end(), [restsym, symbol](Tag sym) {
            return Type::Eq(symbol, sym) || Type::Eq(symbol, restsym);
          });

      if (el != lexicals.end())
        Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                         "duplicate symbol in lambda list (parse-lambda)",
                         lambda);

      if (has_rest) restsym = symbol;

      lexicals.push_back(symbol);
    };

    Cons::cons_iter<Tag> iter(lambda);
    for (auto it = iter.begin(); it != iter.end(); it = ++iter)
      parse(env, it->car);

    if (has_rest && Type::Null(restsym))
      Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                       "early end of lambda list (parse-lambda)", lambda);

    if (lexicals.size() != (Cons::Length(env, lambda) - has_rest))
      Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                       ":rest should terminate lambda list (parse-lambda)",
                       lambda);

    /** * ((lexicals...) . restsym) */
    return Cons(Cons::List(env, lexicals), restsym).Evict(env);
  };

  auto lambda = parse_lambda(env, Cons::car(form));

  auto fn = Function(env, Type::NIL, std::vector<Frame*>{}, lambda,
                     Cons(lambda, Type::NIL).tag_)
                .Evict(env);

  if (Function::arity(fn)) env->lexenv_.push_back(fn);

  Function::form(fn, Cons(lambda, List(env, Cons::cdr(form))).Evict(env));

  if (Function::arity(fn)) env->lexenv_.pop_back();

  return fn;
}

/** * (:defsym symbol form) **/
auto DefSymbol(Env* env, Tag form) {
  if (Cons::Length(env, form) != 3)
    Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     ":defsym: argument count(2)", form);

  auto args = Cons::cdr(form);
  auto sym = Cons::Nth(args, 0);
  auto expr = Cons::Nth(args, 1);

  if (!Symbol::IsType(sym))
    Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "is not a symbol (:defsym)", sym);

  if (Symbol::IsKeyword(sym))
    Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "can't bind keywords (:defsym)", sym);

  if (Symbol::IsBound(sym))
    Condition::Raise(env, Condition::CONDITION_CLASS::CELL_ERROR,
                     "symbol previously bound (:defsym)", sym);

  auto value = Eval(env, Compile(env, expr));
  (void)Symbol::Bind(sym, value);

  if (Function::IsType(value)) Function::name(value, sym);

  return Cons::List(env, std::vector<Tag>{Symbol::Keyword("quote"), sym});
}

/** * (:lambda list . body) **/
auto DefLambda(Env* env, Tag form) {
  if (Cons::Length(env, form) == 1)
    Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     ":lambda: argument count(1*)", form);

  auto args = Cons::cdr(form);
  auto lambda = Cons::Nth(args, 0);

  if (!Cons::IsList(lambda))
    Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR, ":lambda",
                     lambda);

  assert(!Type::Eq(Cons::car(lambda), Symbol::Keyword("quote")));

  return Lambda(env, args);
}

/** * (:macro list . body) **/
auto DefMacro(Env* env, Tag form) {
  if (Cons::Length(env, form) == 1)
    Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     ":macro: argument count(1*)", form);

  auto args = Cons::cdr(form);
  auto lambda = Cons::Nth(args, 0);

  if (!Cons::IsList(lambda))
    Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR, ":macro",
                     lambda);

  assert(!Type::Eq(Cons::car(lambda), Symbol::Keyword("quote")));

  return Macro(Lambda(env, args)).Evict(env);
}

/** * (:letq symbol expr) **/
auto Letq(Env* env, Tag form) {
  if (Cons::Length(env, form) != 3)
    Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     ":letq: argument count(2)", form);

  auto args = Cons::cdr(form);
  auto sym = Cons::Nth(args, 0);
  auto expr = Cons::Nth(args, 1);

  if (!Symbol::IsType(sym))
    Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR, ":letq", sym);

  auto lsym = Compile(env, sym);

  if (!Cons::IsList(lsym))
    Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR, ":letq",
                     lsym);

  auto letq = Namespace::FindInterns(env, env->mu_, String(env, "letq").tag_);
  assert(!Type::Null(letq));

  return Cons::List(
      env, std::vector<Tag>{letq, Cons::Nth(lsym, 1), Cons::Nth(lsym, 2),
                            Compile(env, expr)});
}

/** * (:quote object) **/
auto Quote(Env* env, Tag form) {
  if (Cons::Length(env, form) != 2)
    Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     ":quote: argument count(1)", form);

  return form;
}

/** * (:t object object) **/
auto T(Env* env, Tag form) {
  if (Cons::Length(env, form) != 3)
    Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     ":t: argument count(1)", form);

  return form;
}

/** * (:nil object object) **/
auto Nil(Env* env, Tag form) {
  if (Cons::Length(env, form) != 3)
    Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     ":nil: argument count(1)", form);

  return form;
}

/** * map keyword to handler **/
static const std::map<Tag, std::function<Tag(Env*, Tag)>> kSpecMap{
    {Symbol::Keyword("defsym"), DefSymbol},
    {Symbol::Keyword("lambda"), DefLambda},
    {Symbol::Keyword("letq"), Letq},
    {Symbol::Keyword("macro"), DefMacro},
    {Symbol::Keyword("quote"), Quote},
    {Symbol::Keyword("t"), T},
    {Symbol::Keyword("nil"), Nil}};

} /* anonymous namespace */

/** * special operator predicate **/
auto IsSpecOp(Tag symbol) -> bool {
  return Symbol::IsKeyword(symbol) && (kSpecMap.count(symbol) != 0);
}

/** * compile form **/
auto Compile(Env* env, Tag form) -> Tag {
  Tag rval;

  switch (Type::TypeOf(form)) {
    case SYS_CLASS::CONS: { /* funcall/macro call/special call */
      auto fn = Cons::car(form);

      switch (Type::TypeOf(fn)) {
        case SYS_CLASS::CONS: /* fn is list form */
          rval = List(env, form);
          break;
        case SYS_CLASS::SYMBOL: { /* funcall/macro call/special call */
          Tag lfn;

          std::tie(lfn, std::ignore) = LexicalEnv(env, fn);
          if (Function::IsType(lfn))
            rval = List(env, form);
          else if (Function::IsType(Macro::MacroFunction(env, fn)))
            rval = Compile(env, Macro::MacroExpand(env, form));
          else if (IsSpecOp(fn))
            rval = kSpecMap.at(fn)(env, form);
          else if (!Symbol::IsBound(fn))
            Condition::Raise(env, Condition::CONDITION_CLASS::UNBOUND_VARIABLE,
                             "compile: function symbol", fn);
          else
            rval = List(env, form);
          break;
        }
        case SYS_CLASS::FUNCTION: /* function */
          rval = List(env, form);
          break;
        default:
          Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR,
                           "compile: function type", fn);
          break;
      }
      break;
    }
    case SYS_CLASS::SYMBOL: {
      Tag fn;
      size_t offset;

      std::tie<Tag, size_t>(fn, offset) = LexicalEnv(env, form);

      rval =
          Function::IsType(fn)
              ? Compile(
                    env,
                    Cons::List(
                        env,
                        std::vector<Tag>{
                            Namespace::FindInterns(
                                env, env->mu_, String(env, "frame-ref").tag_),
                            Function::frame_id(fn), Fixnum(offset).tag_}))
              : form;
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
