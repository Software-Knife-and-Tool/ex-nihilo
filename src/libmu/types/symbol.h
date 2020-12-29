/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  symbol.h: library symbols
 **
 **/
#if !defined(_LIBMU_TYPES_SYMBOL_H_)
#define _LIBMU_TYPES_SYMBOL_H_

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
    TagPtr ns;
    TagPtr name;
    TagPtr value;
  } Layout;

  Layout symbol_;

 public:
  static constexpr bool IsType(TagPtr ptr) {
    return (TagOf(ptr) == TAG::SYMBOL) || IsKeyword(ptr);
  }

  /* keywords */
  static constexpr bool IsKeyword(TagPtr ptr) {
    return IsImmediate(ptr) &&
           (ImmediateClass(ptr) == IMMEDIATE_CLASS::KEYWORD);
  }

  static constexpr TagPtr Keyword(TagPtr name) {
    assert(String::IsType(name));

    return MakeImmediate(ImmediateData(name), ImmediateSize(name),
                         IMMEDIATE_CLASS::KEYWORD);
  }

  static TagPtr Keyword(const std::string& name) {
    return Keyword(String::MakeImmediate(name));
  }

  /* accessors */
  static TagPtr ns(TagPtr symbol) {
    assert(IsType(symbol));

    return IsKeyword(symbol) ? NIL : Untag<Layout>(symbol)->ns;
  }

  static void ns(TagPtr, TagPtr);

  static TagPtr value(TagPtr symbol) {
    assert(IsType(symbol));

    return IsKeyword(symbol) ? symbol : Untag<Layout>(symbol)->value;
  }

  static TagPtr name(TagPtr symbol) {
    assert(IsType(symbol));

    return IsKeyword(symbol)
               ? MakeImmediate(ImmediateData(symbol), ImmediateSize(symbol),
                               IMMEDIATE_CLASS::STRING)
               : Untag<Layout>(symbol)->name;
  }

  /** * namespaces **/
  static bool IsUninterned(TagPtr symbol) {
    assert(IsType(symbol) && !IsKeyword(symbol));

    return Null(Symbol::ns(symbol));
  }

  static TagPtr Bind(TagPtr symbol, TagPtr value) {
    assert(IsType(symbol));
    assert(!IsBound(symbol));

    Untag<Layout>(symbol)->value = value;
    return symbol;
  }

  static void GcMark(Env*, TagPtr);
  static bool IsBound(TagPtr);
  static void Print(Env*, TagPtr, TagPtr, bool);
  static TagPtr ParseSymbol(Env*, std::string, bool);
  static TagPtr ViewOf(Env*, TagPtr);

 public: /* object model */
  TagPtr Evict(Env*);

  explicit Symbol(TagPtr, TagPtr);
};

} /* namespace core */
} /* namespace libmu */

#endif /* _LIBMU_TYPES_SYMBOL_H_ */
