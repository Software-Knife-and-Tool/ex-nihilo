/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  char.h: library characters
 **
 **/
#if !defined(_LIBMU_TYPES_CHAR_H_)
#define _LIBMU_TYPES_CHAR_H_

#include <cassert>

#include "libmu/type.h"

namespace libmu {

/** * character type class **/
class Char : public Type {
 public: /* TagPtr */
  static constexpr bool IsType(TagPtr ptr) {
    return IsImmediate(ptr) && (ImmediateClass(ptr) == IMMEDIATE_CLASS::CHAR);
  }

  static constexpr uint8_t Uint8Of(TagPtr ptr) {
    assert(IsType(ptr));

    return static_cast<uint8_t>(ImmediateData(ptr));
  }

  static void Print(Env*, TagPtr, TagPtr, bool);
  static TagPtr Read(Env*, TagPtr);
  static TagPtr ViewOf(Env*, TagPtr);

 public: /* object model */
  TagPtr Evict(Env*, const char*) { return tag_; }

  explicit Char(uint8_t ch) : Type() {
    tag_ = MakeImmediate(ch, 0, IMMEDIATE_CLASS::CHAR);
  }

}; /* class Char */

} /* namespace libmu */

#endif /* _LIBMU_TYPES_CHAR_H_ */
