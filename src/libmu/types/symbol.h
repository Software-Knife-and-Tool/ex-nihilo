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
  } Layout;

  Layout symbol_;
  // TagFmt<Layout>* tagFmt_;

 public:
  static constexpr auto IsType(Tag ptr) -> bool {
    return (TagOf(ptr) == TAG::SYMBOL) || IsKeyword(ptr);
  }

  /* keywords */
  static constexpr auto IsKeyword(Tag ptr) -> bool {
    return IsImmediate(ptr) &&
           (ImmediateClass(ptr) == IMMEDIATE_CLASS::KEYWORD);
  }

  static constexpr auto Keyword(Tag name) -> Tag {
    assert(String::IsType(name));

    return MakeImmediate(ImmediateData(name), ImmediateSize(name),
                         IMMEDIATE_CLASS::KEYWORD);
  }

  static auto Keyword(const std::string& name) -> Tag {
    return Keyword(String::MakeImmediate(name));
  }

  /* accessors */
  static auto ns(Tag symbol) -> Tag {
    assert(IsType(symbol));

    return IsKeyword(symbol) ? NIL : Untag<Layout>(symbol)->ns;
  }

  static auto ns(Tag symbol, Tag ns) -> void;

  static auto value(Tag symbol) -> Tag {
    assert(IsType(symbol));

    return IsKeyword(symbol) ? symbol : Untag<Layout>(symbol)->value;
  }

  static auto name(Tag symbol) -> Tag {
    assert(IsType(symbol));

    return IsKeyword(symbol)
               ? MakeImmediate(ImmediateData(symbol), ImmediateSize(symbol),
                               IMMEDIATE_CLASS::STRING)
               : Untag<Layout>(symbol)->name;
  }

  /** * namespaces **/
  static auto IsUninterned(Tag symbol) -> bool {
    assert(IsType(symbol) && !IsKeyword(symbol));

    return Null(Symbol::ns(symbol));
  }

  static auto Bind(Tag symbol, Tag value) -> Tag {
    assert(IsType(symbol));

    Untag<Layout>(symbol)->value = value;
    return symbol;
  }

  static auto GcMark(Env*, Tag) -> void;
  static auto IsBound(Tag) -> bool;
  static auto Print(Env*, Tag, Tag, bool) -> void;
  static auto ParseSymbol(Env*, std::string, bool) -> Tag;
  static auto ViewOf(Env* env, Tag) -> Tag;

 public: /* type model */
  auto Evict(Env*) -> Tag;
  static auto EvictTag(Env*, Tag) -> Tag;

 public: /* object */
  explicit Symbol(Tag, Tag);
  explicit Symbol(Tag, Tag, Tag);
};

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TYPES_SYMBOL_H_ */
