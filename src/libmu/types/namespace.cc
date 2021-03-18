/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 ** namespace.cc: library namespaces
 **
 **/
#include "libmu/types/namespace.h"

#include <cassert>
#include <vector>

#include "libmu/core.h"
#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/heap/heap.h"

#include "libmu/types/cons.h"
#include "libmu/types/function.h"
#include "libmu/types/string.h"
#include "libmu/types/symbol.h"

namespace libmu {
namespace core {

/** * garbage collect the namespace **/
auto Namespace::GcMark(Env* env, Tag ns) -> void {
  assert(IsType(ns));

  if (!env->heap_->IsGcMarked(ns)) {
    env->heap_->GcMark(ns);
    env->GcMark(env, name(ns));
    env->GcMark(env, imports(ns));
    for (auto entry : externs(ns)) Symbol::GcMark(env, entry.second);
    for (auto entry : interns(ns)) Symbol::GcMark(env, entry.second);
  }
}

/** * view of namespace object **/
auto Namespace::ViewOf(Env* env, Tag ns) -> Tag {
  assert(IsType(ns));

  auto view =
      std::vector<Tag>{Symbol::Keyword("ns"), ns,
                       Fixnum(ToUint64(ns) >> 3).tag_, name(ns), imports(ns)};

  return Vector(env, view).tag_;
}

/** * find symbol in namespace/imports **/
auto Namespace::FindSymbol(Env* env, Tag ns, Tag str) -> Tag {
  assert(IsType(ns));
  assert(String::IsType(str));

  auto sym = FindExterns(ns, str);
  if (!Type::Null(sym)) return sym;

  Cons::cons_iter<Tag> iter(Namespace::imports(ns));
  for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
    auto sym = FindSymbol(env, it->car, str);
    if (!Type::Null(sym)) return sym;
  }

  return Type::NIL;
}

/** * intern extern symbol in namespace **/
auto Namespace::Intern(Env* env, Tag ns, Tag name) -> Tag {
  assert(IsType(ns));
  assert(String::IsType(name));

  auto key = static_cast<Tag>(hash_id(name));
  auto sym = FindSymbol(env, ns, name);

  return Type::Null(sym) ? Insert(Untag<HeapLayout>(ns)->externs, key,
                                  Symbol(ns, name).Evict(env))
                         : sym;
}

/** * intern extern symbol in namespace **/
auto Namespace::Intern(Env* env, Tag ns, Tag name, Tag value) -> Tag {
  assert(IsType(ns));
  assert(String::IsType(name));

  auto key = static_cast<Tag>(hash_id(name));
  auto sym = FindSymbol(env, ns, name);

  return Type::Null(sym) ? Insert(Untag<HeapLayout>(ns)->externs, key,
                                  Symbol(ns, name, value).Evict(env))
                         : sym;
}

/** * intern symbol in namespace **/
auto Namespace::InternInNs(Env* env, Tag ns, Tag name) -> Tag {
  assert(IsType(ns));
  assert(String::IsType(name));

  auto key = static_cast<Tag>(hash_id(name));
  auto sym = FindInterns(ns, name);

  return Type::Null(sym) ? Insert(Untag<HeapLayout>(ns)->interns, key,
                                  Symbol(ns, name).Evict(env))
                         : sym;
}

/** * extern symbol in namespace **/
auto Namespace::ExternInNs(Env* env, Tag ns, Tag name) -> Tag {
  assert(IsType(ns));
  assert(String::IsType(name));

  auto key = static_cast<Tag>(hash_id(name));
  auto sym = FindExterns(ns, name);

  return Type::Null(sym) ? Insert(Untag<HeapLayout>(ns)->externs, key,
                                  Symbol(ns, name).Evict(env))
                         : sym;
}

/** * namespace symbols **/
auto Namespace::Symbols(Env* env, Tag ns) -> Tag {
  assert(IsType(ns));

  auto syms = Untag<HeapLayout>(ns)->externs;
  auto list = *syms.get();

  std::vector<Tag> symv;

  for (auto map : list) symv.push_back(map.second);

  return Cons::List(env, symv);
}

/** evict namespace to heap **/
auto Namespace::Evict(Env* env) -> Tag {
  auto np =
      env->heap_alloc<HeapLayout>(sizeof(HeapLayout), SYS_CLASS::NAMESPACE);

  *np = namespace_;
  tag_ = Entag(np, TAG::EXTEND);

  return tag_;
}

/** * print namespace **/
auto Namespace::Print(Env* env, Tag ns, Tag str, bool) -> void {
  assert(IsType(ns));
  assert(Stream::IsType(str));

  auto stream = Stream::StreamDesignator(env, str);

  auto type =
      String::StdStringOf(Symbol::name(Type::MapClassSymbol(Type::TypeOf(ns))));

  auto name = String::StdStringOf(Namespace::name(ns));

  std::stringstream hexs;

  hexs << std::hex << Type::to_underlying(ns);
  core::PrintStdString(
      env, "#<:" + type + " #x" + hexs.str() + " (" + name + "...)>", stream,
      false);
}

Namespace::Namespace(Tag name, Tag imports) : Type() {
  assert(Cons::IsList(imports));
  assert(String::IsType(name));

  namespace_.name = name;
  namespace_.imports = imports;
  namespace_.externs = std::make_shared<symbol_map>();
  namespace_.interns = std::make_shared<symbol_map>();

  tag_ = Entag(reinterpret_cast<void*>(&namespace_), TAG::EXTEND);
}

} /* namespace core */
} /* namespace libmu */
