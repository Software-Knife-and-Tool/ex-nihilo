/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  char.h: library characters
 **
 **/
#if !defined(LIBMU_TYPES_CHAR_H_)
#define LIBMU_TYPES_CHAR_H_

#include <cassert>

#include "libmu/type.h"

namespace libmu {
namespace core {

using Tag = core::Type::Tag;

/** * character type class **/
class Char : public Type {
 public: /* tag */
  static constexpr auto IsType(Tag ptr) -> bool {
    return IsImmediate(ptr) && (ImmediateClass(ptr) == IMMEDIATE_CLASS::CHAR);
  }

  static constexpr auto Uint8Of(Tag ptr) -> uint8_t {
    assert(IsType(ptr));

    return static_cast<uint8_t>(ImmediateData(ptr));
  }

  static auto VSpecOf(Tag ch) -> float { return Uint8Of(ch); }
  static auto Print(Env*, Tag, Tag, bool) -> void;
  static auto Read(Env*, Tag) -> Tag;
  static auto ViewOf(Env*, Tag) -> Tag;

 public: /* type model */
  auto Evict(Env*) -> Tag { return tag_; }

 public: /* object */
  explicit Char(uint8_t ch) : Type() {
    tag_ = MakeImmediate(ch, 0, IMMEDIATE_CLASS::CHAR);
  }

}; /* class Char */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TYPES_CHAR_H_ */
