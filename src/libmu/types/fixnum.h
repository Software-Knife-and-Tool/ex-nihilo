/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  fixnum.h: library fixnums
 **
 **/
#if !defined(LIBMU_TYPES_FIXNUM_H_)
#define LIBMU_TYPES_FIXNUM_H_

#include <cassert>

#include "libmu/type.h"

namespace libmu {
namespace core {

/** * fixnum type class **/
class Fixnum : public Type {
 public: /* tag */
  static constexpr bool IsType(Tag ptr) {
    return (TagOf(ptr) == TAG::EFIXNUM) || (TagOf(ptr) == TAG::OFIXNUM);
  }

  static constexpr bool IsByte(Tag ptr) {
    return IsType(ptr) && Uint64Of(ptr) < 256;
  }

  static constexpr int64_t Int64Of(Tag fx) {
    return static_cast<int64_t>(
        (static_cast<uint64_t>(fx) >> 2) |
        (((1ull << 63) & static_cast<uint64_t>(fx)) ? (3ull << 62) : 0));
  }

  static constexpr uint64_t Uint64Of(Tag fx) {
    return static_cast<uint64_t>((static_cast<uint64_t>(fx) >> 2));
  }

  static auto VSpecOf(Tag fx) -> int64_t { return Int64Of(fx); }
  static auto Print(Env*, Tag, Tag, bool) -> void;
  static auto ViewOf(Tag) -> Tag;

 public: /* type model */
  Tag Evict(Env*) { return tag_; }

 public: /* object */
  explicit Fixnum(int64_t val) : Type() {
    tag_ = Entag(static_cast<Tag>(val << 2), TAG::EFIXNUM);
  }
};

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TYPES_FIXNUM_H_ */
