/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  namespace.h: library symbol bindings
 **
 **/
#if !defined(_LIBMU_TYPES_NAMESPACE_H_)
#define _LIBMU_TYPES_NAMESPACE_H_

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

/** * namespace type class **/
class Namespace : public Type {
 private: /* symbol map, keyed by hash id */
  /* think: wouldn't it be less stress to key by symbol TagPtr? */
  typedef std::unordered_map<uint32_t, TagPtr> symbol_map;
  typedef symbol_map::const_iterator symbol_iter;

  static const size_t FNV_prime = 16777619;
  static const uint64_t OFFSET_BASIS = 2166136261UL;

  static symbol_iter find(const std::shared_ptr<symbol_map>& map, TagPtr key) {
    return map->find(static_cast<uint64_t>(key));
  }

  static bool isFound(const std::shared_ptr<symbol_map>& map, symbol_iter el) {
    return el != map->end();
  }

  static TagPtr Insert(const std::shared_ptr<symbol_map>& map, TagPtr key,
                       TagPtr symbol) {
    (*map.get())[static_cast<uint64_t>(key)] = symbol;
    return symbol;
  }

  static uint64_t hash_id(TagPtr str) {
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
    TagPtr name;
    TagPtr import;
    std::shared_ptr<symbol_map> externs;
    std::shared_ptr<symbol_map> interns;
  } Layout;

  Layout namespace_;

 public: /* TagPtr */
  static constexpr bool IsType(TagPtr ptr) {
    return IsExtended(ptr) && Heap::SysClass(ptr) == SYS_CLASS::NAMESPACE;
  }

  /** * accessor **/
  static TagPtr name(TagPtr ns) {
    assert(IsType(ns));

    return Untag<Layout>(ns)->name;
  }

  /** * accessors **/
  static TagPtr import(TagPtr ns) {
    assert(IsType(ns));

    return Untag<Layout>(ns)->import;
  }

  static symbol_map externs(TagPtr ns) {
    assert(IsType(ns));

    return *Untag<Layout>(ns)->externs.get();
  }

  static symbol_map interns(TagPtr ns) {
    assert(IsType(ns));

    return *Untag<Layout>(ns)->interns.get();
  }

  /** * find symbol in namespace externs **/
  static TagPtr FindInNsExterns(Env*, TagPtr ns, TagPtr str) {
    assert(IsType(ns));
    assert(String::IsType(str));

    auto key = static_cast<TagPtr>(hash_id(str));
    auto entry = find(Untag<Layout>(ns)->externs, key);

    return isFound(Untag<Layout>(ns)->externs, entry) ? entry->second
                                                      : Type::NIL;
  }

  /** * find symbol in namespace interns **/
  static TagPtr FindInNsInterns(Env*, TagPtr ns, TagPtr str) {
    assert(IsType(ns));
    assert(String::IsType(str));

    auto key = static_cast<TagPtr>(hash_id(str));
    auto entry = find(Untag<Layout>(ns)->interns, key);

    return isFound(Untag<Layout>(ns)->interns, entry) ? entry->second
                                                      : Type::NIL;
  }

  /** * find symbol in namespace externs **/
  static TagPtr FindInNsExterns(Env* env, TagPtr ns, std::string str) {
    assert(IsType(ns));

    return FindInNsExterns(env, ns, String(env, str).tag_);
  }

  /** * find symbol in namespace interns **/
  static TagPtr FindInNsInterns(Env* env, TagPtr ns, std::string str) {
    assert(IsType(ns));

    return FindInNsInterns(env, ns, String(env, str).tag_);
  }

  static TagPtr Symbols(Env*, TagPtr);
  static void GcMark(Env*, TagPtr);
  static TagPtr Intern(Env*, TagPtr, TagPtr);
  static TagPtr InternInNs(Env*, TagPtr, TagPtr);
  static TagPtr ExternInNs(Env*, TagPtr, TagPtr);
  static TagPtr FindSymbol(Env*, TagPtr, TagPtr);
  static TagPtr ViewOf(Env*, TagPtr);

  static void Print(Env*, TagPtr, TagPtr, bool);
  
 public: /* object model */
  TagPtr Evict(Env*, const char*);

  explicit Namespace(TagPtr, TagPtr);
}; /* class Namespace */

} /* namespace libmu */

#endif /* _LIBMU_TYPES_NAMESPACE_H_ */
