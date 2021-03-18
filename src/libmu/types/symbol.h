/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  symbol.h: library symbols
 **
 **/
#if !defined(LIBMU_TYPES_SYMBOL_H_)
#define LIBMU_TYPES_SYMBOL_H_

#include <cassert>

#include "libmu/readtable.h"
#include "libmu/type.h"

#include "libmu/types/stream.h"
#include "libmu/types/string.h"

namespace libmu {
namespace core {

class Env;

/** * symbol type class **/
class Symbol : public Type {
 private:
  typedef struct {
    Tag ns;
    Tag name;
    Tag value;
  } HeapLayout;

  HeapLayout symbol_;

 public:
  static constexpr bool IsType(Tag ptr) {
    return (TagOf(ptr) == TAG::SYMBOL) || IsKeyword(ptr);
  }

  /* keywords */
  static constexpr bool IsKeyword(Tag ptr) {
    return IsImmediate(ptr) &&
           (ImmediateClass(ptr) == IMMEDIATE_CLASS::KEYWORD);
  }

  static constexpr Tag Keyword(Tag name) {
    assert(String::IsType(name));

    return MakeImmediate(ImmediateData(name), ImmediateSize(name),
                         IMMEDIATE_CLASS::KEYWORD);
  }

  static Tag Keyword(const std::string& name) {
    return Keyword(String::MakeImmediate(name));
  }

  /* accessors */
  static Tag ns(Tag symbol) {
    assert(IsType(symbol));

    return IsKeyword(symbol) ? NIL : Untag<HeapLayout>(symbol)->ns;
  }

  static void ns(Tag, Tag);

  static Tag value(Tag symbol) {
    assert(IsType(symbol));

    return IsKeyword(symbol) ? symbol : Untag<HeapLayout>(symbol)->value;
  }

  static Tag name(Tag symbol) {
    assert(IsType(symbol));

    return IsKeyword(symbol)
               ? MakeImmediate(ImmediateData(symbol), ImmediateSize(symbol),
                               IMMEDIATE_CLASS::STRING)
               : Untag<HeapLayout>(symbol)->name;
  }

  /** * namespaces **/
  static bool IsUninterned(Tag symbol) {
    assert(IsType(symbol) && !IsKeyword(symbol));

    return Null(Symbol::ns(symbol));
  }

  static Tag Bind(Tag symbol, Tag value) {
    assert(IsType(symbol));

    Untag<HeapLayout>(symbol)->value = value;
    return symbol;
  }

  static void GcMark(Env*, Tag);
  static bool IsBound(Tag);
  static void Print(Env*, Tag, Tag, bool);
  static Tag ParseSymbol(Env*, std::string, bool);
  static Tag ViewOf(Env*, Tag);

 public: /* object model */
  Tag Evict(Env*);

  explicit Symbol(Tag, Tag);
  explicit Symbol(Tag, Tag, Tag);
};

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TYPES_SYMBOL_H_ */
