/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 ** env.cc: skein environment
 **
 **/
#include <cassert>
#include <vector>

namespace skein {

/** * find symbol in namespace/imports **/
auto Env::Find(Env* env, Tag ns, Tag str) -> Tag {
  assert(IsType(ns));
  assert(String::IsType(str));

  auto sym = FindExterns(ns, str);
  if (!Type::Null(sym)) return sym;

  Cons::cons_iter<Tag> iter(Env::imports(ns));
  for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
    auto sym = FindSymbol(env, it->car, str);
    if (!Type::Null(sym)) return sym;
  }

  return Type::NIL;
}

/** * intern extern symbol in namespace **/
auto Env::Intern(Env* env, Tag ns, Tag name) -> Tag {
  assert(IsType(ns));
  assert(String::IsType(name));

  auto key = static_cast<Tag>(hash_id(name));
  auto sym = FindSymbol(env, ns, name);

  /* symbols assigned to namespaces are automatically evicted */
  return Type::Null(sym) ? Insert(Untag<Layout>(ns)->externs, key,
                                  Symbol(ns, name).Evict(env))
                         : sym;
}

/** * intern extern symbol in namespace **/
auto Env::Intern(Env* env, Tag ns, Tag name, Tag value) -> Tag {
  assert(IsType(ns));
  assert(String::IsType(name));

  auto key = static_cast<Tag>(hash_id(name));
  auto sym = FindSymbol(env, ns, name);

  /* symbols assigned to namespaces are automatically evicted */
  auto foo = Type::Null(sym) ? Insert(Untag<Layout>(ns)->externs, key,
                                      Symbol(ns, name, value).Evict(env))
                             : sym;
  return foo;
}

/** * intern symbol in namespace **/
auto Env::InternInNs(Env* env, Tag ns, Tag name) -> Tag {
  assert(IsType(ns));
  assert(String::IsType(name));

  auto key = static_cast<Tag>(hash_id(name));
  auto sym = FindInterns(ns, name);

  /* symbols assigned to namespaces are automatically evicted */
  return Type::Null(sym) ? Insert(Untag<Layout>(ns)->interns, key,
                                  Symbol(ns, name).Evict(env))
                         : sym;
}

/** * extern symbol in namespace **/
auto Env::ExternInNs(Env* env, Tag ns, Tag name) -> Tag {
  assert(IsType(ns));
  assert(String::IsType(name));

  auto key = static_cast<Tag>(hash_id(name));
  auto sym = FindExterns(ns, name);

  /* symbols assigned to namespaces are automatically evicted */
  return Type::Null(sym) ? Insert(Untag<Layout>(ns)->externs, key,
                                  Symbol(ns, name).Evict(env))
                         : sym;
}

/** * namespace symbols **/
auto Env::Symbols(Env* env, Tag ns) -> Tag {
  assert(IsType(ns));

  auto syms = Untag<Layout>(ns)->externs;
  auto list = *syms.get();

  std::vector<Tag> symv;

  for (auto map : list) symv.push_back(map.second);

  return Cons::List(env, symv);
}

/** evict namespace to heap **/
auto Env::Evict(Env* env) -> Tag {
  auto hp = env->heap_alloc<Layout>(sizeof(Layout), SYS_CLASS::NAMESPACE);

  *hp = namespace_;
  hp->name = Env::Evict(env, hp->name);
  hp->imports = Env::Evict(env, hp->imports);

  tag_ = Entag(hp, TAG::EXTEND);

  return tag_;
}

auto Env::EvictTag(Env* env, Tag ns) -> Tag {
  assert(IsType(ns));
  assert(!Env::IsEvicted(env, ns));

  // printf("EvictTag: namespace\n");
  auto hp = env->heap_alloc<Layout>(sizeof(Layout), SYS_CLASS::NAMESPACE);
  auto np = Untag<Layout>(ns);

  *hp = *np;
  hp->name = Env::Evict(env, hp->name);
  hp->imports = Env::Evict(env, hp->imports);

  return Entag(hp, TAG::EXTEND);
}

/** * print namespace **/
auto Env::Print(Env* env, Tag ns, Tag str, bool) -> void {
  assert(IsType(ns));
  assert(Stream::IsType(str));

  auto stream = Stream::StreamDesignator(env, str);

  auto type =
      String::StdStringOf(Symbol::name(Type::MapClassSymbol(Type::TypeOf(ns))));

  auto name = String::StdStringOf(Env::name(ns));

  std::stringstream hexs;

  hexs << std::hex << Type::to_underlying(ns);
  core::PrintStdString(
      env, "#<:" + type + " #x" + hexs.str() + " (" + name + "...)>", stream,
      false);
}

Env::Env(Tag name, Tag imports) : Type() {
  assert(Cons::IsList(imports));
  assert(String::IsType(name));

  namespace_.name = name;
  namespace_.imports = imports;
  namespace_.externs = std::make_shared<symbol_map>();
  namespace_.interns = std::make_shared<symbol_map>();

  tagFormat_ =
      new TagFormat<Layout>(SYS_CLASS::NAMESPACE, TAG::EXTEND, &namespace_);
  tag_ = tagFormat_->tag_;
}

} /* namespace core */
} /* namespace libmu */
