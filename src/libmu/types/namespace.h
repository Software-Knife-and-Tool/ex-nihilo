/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
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
#include "libmu/env.h"
#include "libmu/eval.h"
#include "libmu/print.h"
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

  static symbol_iter find(const std::shared_ptr<symbol_map>& map, Tag key) {
    return map->find(static_cast<uint64_t>(key));
  }

  static bool isFound(const std::shared_ptr<symbol_map>& map, symbol_iter el) {
    return el != map->end();
  }

  static Tag Insert(const std::shared_ptr<symbol_map>& map, Tag key,
                    Tag symbol) {
    (*map.get())[static_cast<uint64_t>(key)] = symbol;
    return symbol;
  }

  static uint64_t hash_id(Tag str) {
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

 public: /* Tag */
  static constexpr bool IsType(Tag ptr) {
    return IsExtended(ptr) && Heap::SysClass(ptr) == SYS_CLASS::NAMESPACE;
  }

  /** * accessor **/
  static Tag name(Tag ns) {
    assert(IsType(ns));

    return Untag<Layout>(ns)->name;
  }

  /** * accessors **/
  static Tag imports(Tag ns) {
    assert(IsType(ns));

    return Untag<Layout>(ns)->imports;
  }

  static symbol_map externs(Tag ns) {
    assert(IsType(ns));

    return *Untag<Layout>(ns)->externs.get();
  }

  static symbol_map interns(Tag ns) {
    assert(IsType(ns));

    return *Untag<Layout>(ns)->interns.get();
  }

  /** * is in namespace externs? **/
  static bool IsExtern(Tag ns, Tag name) {
    assert(IsType(ns));
    assert(String::IsType(name));

    return !Null(Namespace::FindExterns(ns, name));
  }

  /** * is in namespace interns? **/
  static bool IsIntern(Tag ns, Tag name) {
    assert(IsType(ns));
    assert(String::IsType(name));

    return !Null(Namespace::FindInterns(ns, name));
  }

  /** * find symbol in namespace externs **/
  static Tag FindExterns(Tag ns, Tag str) {
    assert(IsType(ns));
    assert(String::IsType(str));

    auto key = static_cast<Tag>(hash_id(str));
    auto entry = find(Untag<Layout>(ns)->externs, key);

    return isFound(Untag<Layout>(ns)->externs, entry) ? entry->second
                                                      : Type::NIL;
  }

  /** * find symbol in namespace interns **/
  static Tag FindInterns(Tag ns, Tag str) {
    assert(IsType(ns));
    assert(String::IsType(str));

    auto key = static_cast<Tag>(hash_id(str));
    auto entry = find(Untag<Layout>(ns)->interns, key);

    return isFound(Untag<Layout>(ns)->interns, entry) ? entry->second
                                                      : Type::NIL;
  }

  static Tag Symbols(Env*, Tag);
  static void GcMark(Env*, Tag);
  static Tag Intern(Env*, Tag, Tag);
  static Tag Intern(Env*, Tag, Tag, Tag);
  static Tag InternInNs(Env*, Tag, Tag);
  static Tag ExternInNs(Env*, Tag, Tag);
  static Tag FindSymbol(Env*, Tag, Tag);
  static Tag ViewOf(Env*, Tag);

  static void Print(Env*, Tag, Tag, bool);

 public: /* object model */
  Tag Evict(Env*);

  explicit Namespace(Tag, Tag);
}; /* class Namespace */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TYPES_NAMESPACE_H_ */
