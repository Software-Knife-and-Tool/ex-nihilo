/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 ** symbol.cc: library symbols
 **
 **/
#include "libmu/types/symbol.h"

#include <cassert>

#include "libmu/core.h"
#include "libmu/env.h"
#include "libmu/readtable.h"
#include "libmu/type.h"

#include "libmu/types/cons.h"

#include "libmu/types/condition.h"
#include "libmu/types/function.h"
#include "libmu/types/namespace.h"
#include "libmu/types/stream.h"
#include "libmu/types/string.h"

namespace libmu {
namespace core {
namespace {

/** * parse symbol namespace designator **/
auto NamespaceOf(Env* env, const std::string& symbol, const std::string& sep)
    -> Tag {
  auto cpos = symbol.find(sep);
  Tag ns;

  if (cpos < symbol.size()) {
    ns = Env::MapNamespace(env, symbol.substr(0, cpos));

    if (Type::Null(ns))
      Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                       "unmapped namespace",
                       String(symbol.substr(0, cpos)).tag_);
  } else {
    ns = Type::NIL;
  }

  return ns;
}

/** * parse symbol name string **/
auto NameOf(Env* env, const std::string& symbol, const std::string& sep)
    -> Tag {
  auto cpos = symbol.find(sep);

  return String((cpos < symbol.size()) ? symbol.substr(cpos + sep.length())
                                       : symbol)
      .tag_;
}

} /* anonymous namespace */

/** * view of symbol object **/
auto Symbol::ViewOf(Tag symbol) -> Tag {
  assert(IsType(symbol));

  auto view = std::vector<Tag>{Symbol::Keyword("symbol"),
                               symbol,
                               Fixnum(ToUint64(symbol) >> 3).tag_,
                               name(symbol),
                               ns(symbol),
                               value(symbol)};

  return VectorT<Tag>(view).tag_;
}

/** * garbage collection **/
auto Symbol::GcMark(Env* env, Tag symbol) -> void {
  assert(IsType(symbol));

  if (!IsKeyword(symbol) && !env->heap_->IsGcMarked(symbol)) {
    env->heap_->GcMark(symbol);
    env->GcMark(env, name(symbol));
    env->GcMark(env, value(symbol));
  }
}

/** * set namespace */
auto Symbol::ns(Tag symbol, Tag ns) -> void {
  assert(IsType(symbol));
  assert(!IsKeyword(symbol));
  assert(Namespace::IsType(ns));

  Untag<HeapLayout>(symbol)->ns = ns;
}

/** * is symbol bound to a value? */
auto Symbol::IsBound(Tag sym) -> bool {
  assert(IsType(sym));

  return IsKeyword(sym) ||
         !Eq(value(sym), static_cast<Tag>(core::SYNTAX_CHAR::UNBOUND));
}

/** * print symbol to stream **/
auto Symbol::Print(Env* env, Tag sym, Tag stream, bool esc) -> void {
  assert(IsType(sym));
  assert(Stream::IsType(stream));

  if (IsKeyword(sym)) {
    core::PrintStdString(env, ":", stream, false);
  } else if (esc) {
    auto ns = Symbol::ns(sym);

    if (Null(ns)) {
      core::PrintStdString(env, "#:", stream, false);
    } else {
      core::PrintStdString(env, String::StdStringOf(Namespace::name(ns)),
                           stream, false);

      if (Namespace::IsExtern(ns, Symbol::name(sym)))
        core::PrintStdString(env, ":", stream, false);
      else if (Namespace::IsIntern(ns, Symbol::name(sym)))
        core::PrintStdString(env, "::", stream, false);
      else
        assert(false);
    }
  }

  core::PrintStdString(env, String::StdStringOf(Symbol::name(sym)), stream,
                       false);
}

/** * parse symbol **/
auto Symbol::ParseSymbol(Env* env, std::string string, bool intern) -> Tag {
  Tag rval;

  if (string.size() == 0)
    Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                     "naked symbol syntax (read)", Type::NIL);

  if (string.size() == 1 && string[0] == '.')
    return static_cast<Tag>(core::SYNTAX_CHAR::DOT);

  auto ch = string[0];
  auto keywdp = ch == ':';

  if (keywdp) {
    if (string.size() == 1)
      Condition::Raise(env, Condition::CONDITION_CLASS::END_OF_FILE,
                       "early eof in keyword (read)", Type::NIL);

    if (string.size() - 1 > Type::IMMEDIATE_STR_MAX)
      Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                       "keyword symbols may not exceed seven characters",
                       String(string).tag_);

    auto key = string;
    rval = Symbol::Keyword(key.erase(0, 1));
  } else {
    auto int_ns = NamespaceOf(env, string, "::");
    auto ext_ns = NamespaceOf(env, string, ":");

    if (intern) {
      if (!Null(int_ns))
        rval = Namespace::InternInNs(env, int_ns, NameOf(env, string, "::"));
      else if (!Null(ext_ns))
        rval = Namespace::ExternInNs(env, ext_ns, NameOf(env, string, ":"));
      else {
        auto name = String(string).tag_;
        rval = Namespace::FindInterns(env->namespace_, name);
        if (Null(rval)) rval = Namespace::Intern(env, env->namespace_, name);
      }
    } else if (Null(ext_ns) && Null(int_ns)) {
      auto name = String(string).tag_;
      rval = Symbol(NIL, name).Evict(env);
    } else
      Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                       "uninterned symbols may not be qualified (read)",
                       String(string).tag_);
  }

  return rval;
}

/** evict symbol to heap **/
auto Symbol::Evict(Env* env) -> Tag {
  auto hp = env->heap_alloc<HeapLayout>(sizeof(HeapLayout), SYS_CLASS::SYMBOL);

  *hp = symbol_;
  hp->ns = Env::Evict(env, hp->ns);
  hp->name = Env::Evict(env, hp->name);
  hp->value = Env::Evict(env, hp->value);

  tag_ = Entag(hp, TAG::SYMBOL);

  return tag_;
}

auto Symbol::EvictTag(Env* env, Tag symbol) -> Tag {
  assert(IsType(symbol));
  assert(!Env::IsEvicted(env, symbol));

  // printf("EvictTag: symbol\n");
  auto hp = env->heap_alloc<HeapLayout>(sizeof(HeapLayout), SYS_CLASS::SYMBOL);
  auto sp = Untag<HeapLayout>(symbol);

  *hp = *sp;
  hp->ns = Env::Evict(env, hp->ns);
  hp->name = Env::Evict(env, hp->name);
  hp->value = Env::Evict(env, hp->value);

  return Entag(hp, TAG::SYMBOL);
}

/** * allocate an unbound symbol from the heap **/
Symbol::Symbol(Tag ns, Tag name) {
  assert(String::IsType(name));
  assert(Namespace::IsType(ns) || Null(ns));

  symbol_.ns = ns;
  symbol_.name = name;
  symbol_.value = static_cast<Tag>(core::SYNTAX_CHAR::UNBOUND);

  tag_ = Type::Entag(reinterpret_cast<void*>(&symbol_), TAG::SYMBOL);
}

/** * allocate an unbound symbol from the heap **/
Symbol::Symbol(Tag ns, Tag name, Tag value) {
  assert(String::IsType(name));
  assert(Namespace::IsType(ns) || Null(ns));

  symbol_.ns = ns;
  symbol_.name = name;
  symbol_.value = value;

  tag_ = Type::Entag(reinterpret_cast<void*>(&symbol_), TAG::SYMBOL);
}

} /* namespace core */
} /* namespace libmu */
