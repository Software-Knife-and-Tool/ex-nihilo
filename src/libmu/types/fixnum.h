/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  fixnum.h: library fixnums
 **
 **/
#if !defined(_LIBMU_TYPES_FIXNUM_H_)
#define _LIBMU_TYPES_FIXNUM_H_

#include <cassert>

#include "libmu/type.h"

namespace libmu {

/** * fixnum type class **/
class Fixnum : public Type {
 public: /* TagPtr */
  static constexpr bool IsType(TagPtr ptr) {
    return (TagOf(ptr) == TAG::EFIXNUM) || (TagOf(ptr) == TAG::OFIXNUM);
  }

  static constexpr bool IsByte(TagPtr ptr) {
    return IsType(ptr) && Uint64Of(ptr) < 256;
  }

  static constexpr int64_t Int64Of(TagPtr fx) {
    return static_cast<int64_t>(
        (static_cast<uint64_t>(fx) >> 2) |
        (((1ull << 63) & static_cast<uint64_t>(fx)) ? (3ull << 62) : 0));
  }

  static constexpr uint64_t Uint64Of(TagPtr fx) {
    return static_cast<uint64_t>((static_cast<uint64_t>(fx) >> 2));
  }

  static void Print(Env*, TagPtr, TagPtr, bool);
  static TagPtr ViewOf(Env*, TagPtr);

 public: /* object model */
  TagPtr Evict(Env*, const char*) { return tag_; }

  explicit Fixnum(int64_t val) : Type() { 
    tag_ = Entag(static_cast<TagPtr>(val << 2), TAG::EFIXNUM);
  }
};

} /* namespace libmu */

#endif /* _LIBMU_TYPES_FIXNUM_H_ */
