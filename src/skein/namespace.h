/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  symbol.h: symbol bindings
 **
 **/
#if !defined(SKEIN_SYMBOL_H_)
#define SKEIN_SYMBOL_H_

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

namespace skein {
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

  static auto Symbols(Env*, Tag) -> Tag;
  static auto Intern(Env*, Tag, Tag) -> Tag;
  static auto Intern(Env*, Tag, Tag, Tag) -> Tag;
  static auto InternInNs(Env*, Tag, Tag) -> Tag;
  static auto ExternInNs(Env*, Tag, Tag) -> Tag;
  static auto FindSymbol(Env*, Tag, Tag) -> Tag;
  
}; /* class Symbol */

} /* namespace core */
} /* namespace libmu */

#endif /* SKEIN_SYMBOL_H_ */
