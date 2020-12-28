/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
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

#include "libmu/env.h"
#include "libmu/print.h"
#include "libmu/type.h"

#include "libmu/heap/heap.h"

#include "libmu/types/cons.h"
#include "libmu/types/function.h"
#include "libmu/types/string.h"
#include "libmu/types/symbol.h"

namespace libmu {
namespace core {

/** * garbage collect the namespace **/
void Namespace::GcMark(Env* env, TagPtr ns) {
  assert(IsType(ns));

  if (!env->heap_->IsGcMarked(ns)) {
    env->heap_->GcMark(ns);
    env->GcMark(env, import(ns));
    for (auto entry : externs(ns)) Symbol::GcMark(env, entry.second);
    for (auto entry : interns(ns)) Symbol::GcMark(env, entry.second);
  }
}

/** * view of namespace object **/
TagPtr Namespace::ViewOf(Env* env, TagPtr ns) {
  assert(IsType(ns));

  auto view =
      std::vector<TagPtr>{Symbol::Keyword("ns"), ns,
                          Fixnum(ToUint64(ns) >> 3).tag_, name(ns), import(ns)};

  return Vector(env, view).tag_;
}

/** * find symbol in namespace/imports **/
TagPtr Namespace::FindSymbol(Env* env, TagPtr ns, TagPtr str) {
  assert(IsType(ns));
  assert(String::IsType(str));

  for (; !Type::Null(ns); ns = Namespace::import(ns)) {
    auto sym = Namespace::FindInExterns(env, ns, str);
    if (!Type::Null(sym)) return sym;
  }

  return Type::NIL;
}

/** * intern extern symbol in namespace **/
TagPtr Namespace::Intern(Env* env, TagPtr ns, TagPtr name) {
  assert(IsType(ns));
  assert(String::IsType(name));

  auto key = static_cast<TagPtr>(hash_id(name));
  auto sym = FindSymbol(env, ns, name);

  return Type::Null(sym) ? Insert(Untag<Layout>(ns)->externs, key,
                                  Symbol(ns, name).Evict(env))
                         : sym;
}

/** * intern symbol in namespace **/
TagPtr Namespace::InternInNs(Env* env, TagPtr ns, TagPtr name) {
  assert(IsType(ns));
  assert(String::IsType(name));

  auto key = static_cast<TagPtr>(hash_id(name));
  auto sym = FindInInterns(env, ns, name);

  return Type::Null(sym) ? Insert(Untag<Layout>(ns)->interns, key,
                                  Symbol(ns, name).Evict(env))
                         : sym;
}

/** * extern symbol in namespace **/
TagPtr Namespace::ExternInNs(Env* env, TagPtr ns, TagPtr name) {
  assert(IsType(ns));
  assert(String::IsType(name));

  auto key = static_cast<TagPtr>(hash_id(name));
  auto sym = FindInExterns(env, ns, name);

  return Type::Null(sym) ? Insert(Untag<Layout>(ns)->externs, key,
                                  Symbol(ns, name).Evict(env))
                         : sym;
}

/** * namespace symbols **/
TagPtr Namespace::Symbols(Env* env, TagPtr ns) {
  assert(IsType(ns));

  auto syms = Untag<Layout>(ns)->externs;
  auto list = *syms.get();

  std::vector<TagPtr> symv;

  for (auto map : list) symv.push_back(map.second);

  return Cons::List(env, symv);
}

/** evict namespace to heap **/
TagPtr Namespace::Evict(Env* env) {
  auto np = env->heap_alloc<Layout>(sizeof(Layout), SYS_CLASS::NAMESPACE);

  *np = namespace_;
  tag_ = Entag(np, TAG::EXTEND);

  return tag_;
}

void Namespace::Print(Env* env, TagPtr ns, TagPtr str, bool) {
  assert(IsType(ns));
  assert(Stream::IsType(str));

  auto stream = Stream::StreamDesignator(env, str);

  auto type =
      String::StdStringOf(Symbol::name(Type::MapClassSymbol(Type::TypeOf(ns))));

  auto name = String::StdStringOf(Namespace::name(ns));

  std::stringstream hexs;

  hexs << std::hex << Type::to_underlying(ns);
  core::PrintStdString(env,
                       "#<:" + type + " #x" + hexs.str() + " (" + name + ")>",
                       stream, false);
}

/** * allocate namespace **/
Namespace::Namespace(TagPtr name, TagPtr import) : Type() {
  assert(String::IsType(name));
  assert(Null(import) || IsType(import));

  namespace_.name = name;
  namespace_.import = import;
  namespace_.externs = std::make_shared<symbol_map>();
  namespace_.interns = std::make_shared<symbol_map>();

  tag_ = Entag(reinterpret_cast<void*>(&namespace_), TAG::EXTEND);
}

} /* namespace core */
} /* namespace libmu */
