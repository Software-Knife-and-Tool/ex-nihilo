/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  namespace.h: library symbol bindings
 **
 **/
#if !defined(LIBMU_TYPES_NAMESPACE_H_)
#define LIBMU_TYPES_NAMESPACE_H_

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

#include "libmu/compiler.h"
#include "libmu/core.h"
#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/heap/heap.h"

#include "libmu/types/char.h"
#include "libmu/types/cons.h"
#include "libmu/types/stream.h"
#include "libmu/types/string.h"
#include "libmu/types/symbol.h"
#include "libmu/types/vector.h"

namespace libmu {
namespace core {

/** * namespace type class **/
class Namespace : public Type {
 private: /* symbol map, keyed by hash id */
  /* think: wouldn't it be less stress to key by symbol Tag? */
  typedef std::unordered_map<uint32_t, Tag> symbol_map;
  typedef symbol_map::const_iterator symbol_iter;

  static const size_t FNV_prime = 16777619;
  static const uint64_t OFFSET_BASIS = 2166136261UL;

  static auto find(const std::shared_ptr<symbol_map>& map, Tag key)
      -> symbol_iter {
    return map->find(static_cast<uint64_t>(key));
  }

  static auto isFound(const std::shared_ptr<symbol_map>& map, symbol_iter el)
      -> bool {
    return el != map->end();
  }

  static auto Insert(const std::shared_ptr<symbol_map>& map, Tag key,
                     Tag symbol) -> Tag {
    (*map.get())[static_cast<uint64_t>(key)] = symbol;
    return symbol;
  }

  static auto hash_id(Tag str) -> uint64_t {
    assert(String::IsType(str));

    uint64_t hash = OFFSET_BASIS;

    Vector::vector_iter<char> iter(str);
    for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
      hash ^= *it;
      hash *= FNV_prime;
    }

    return hash;
  }

 private:
  typedef struct {
    Tag name;    /* string */
    Tag imports; /* list of namespaces */
    std::shared_ptr<symbol_map> externs;
    std::shared_ptr<symbol_map> interns;
  } Layout;

  Layout namespace_;
  TagFormat<Layout>* tagFormat_;

 public: /* Tag */
  static constexpr auto IsType(Tag ptr) -> bool {
    return IsExtended(ptr) &&
           TagFormat<Layout>::SysClass(ptr) == SYS_CLASS::NAMESPACE;
  }

  /** * accessor **/
  static auto name(Tag ns) -> Tag {
    assert(IsType(ns));

    return Untag<Layout>(ns)->name;
  }

  /** * accessors **/
  static auto imports(Tag ns) -> Tag {
    assert(IsType(ns));

    return Untag<Layout>(ns)->imports;
  }

  static auto externs(Tag ns) -> symbol_map {
    assert(IsType(ns));

    return *Untag<Layout>(ns)->externs.get();
  }

  static auto interns(Tag ns) -> symbol_map {
    assert(IsType(ns));

    return *Untag<Layout>(ns)->interns.get();
  }

  /** * is in namespace externs? **/
  static auto IsExtern(Tag ns, Tag name) -> bool {
    assert(IsType(ns));
    assert(String::IsType(name));

    return !Null(Namespace::FindExterns(ns, name));
  }

  /** * is in namespace interns? **/
  static auto IsIntern(Tag ns, Tag name) -> bool {
    assert(IsType(ns));
    assert(String::IsType(name));

    return !Null(Namespace::FindInterns(ns, name));
  }

  /** * find symbol in namespace externs **/
  static auto FindExterns(Tag ns, Tag str) -> Tag {
    assert(IsType(ns));
    assert(String::IsType(str));

    auto key = static_cast<Tag>(hash_id(str));
    auto entry = find(Untag<Layout>(ns)->externs, key);

    return isFound(Untag<Layout>(ns)->externs, entry) ? entry->second
                                                      : Type::NIL;
  }

  /** * find symbol in namespace interns **/
  static auto FindInterns(Tag ns, Tag str) -> Tag {
    assert(IsType(ns));
    assert(String::IsType(str));

    auto key = static_cast<Tag>(hash_id(str));
    auto entry = find(Untag<Layout>(ns)->interns, key);

    return isFound(Untag<Layout>(ns)->interns, entry) ? entry->second
                                                      : Type::NIL;
  }

  static auto Symbols(Env*, Tag) -> Tag;
  static auto GcMark(Env*, Tag) -> void;
  static auto Intern(Env*, Tag, Tag) -> Tag;
  static auto Intern(Env*, Tag, Tag, Tag) -> Tag;
  static auto InternInNs(Env*, Tag, Tag) -> Tag;
  static auto ExternInNs(Env*, Tag, Tag) -> Tag;
  static auto FindSymbol(Env*, Tag, Tag) -> Tag;
  static auto ViewOf(Env* env, Tag) -> Tag;

  static auto Print(Env*, Tag, Tag, bool) -> void;

 public: /* type model */
  auto Evict(Env*) -> Tag;
  static auto EvictTag(Env*, Tag) -> Tag;

 public: /* object */
  explicit Namespace(Tag, Tag);
}; /* class Namespace */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TYPES_NAMESPACE_H_ */
