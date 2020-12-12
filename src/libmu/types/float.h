/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  float.h: library floats
 **
 **/
#if !defined(_LIBMU_TYPES_FLOAT_H_)
#define _LIBMU_TYPES_FLOAT_H_

#include <cassert>
#include <cstring>

#include "libmu/type.h"

namespace libmu {

/** * float type class **/
class Float : public Type {
 public: /* TagPtr */
  static constexpr bool IsType(TagPtr ptr) {
    return IsImmediate(ptr) && (ImmediateClass(ptr) == IMMEDIATE_CLASS::FLOAT);
  }

  static float FloatOf(TagPtr fl) {
    assert(IsType(fl));

    float buf = 0.0;

    std::memcpy(reinterpret_cast<char*>(&buf), reinterpret_cast<char*>(&fl) + 4,
                sizeof(float));
    return buf;
  }

  static float VSpecOf(TagPtr fl) { return FloatOf(fl); }
  static void Print(Env*, TagPtr, TagPtr, bool);
  static TagPtr ViewOf(Env*, TagPtr);

 public: /* object model */
  TagPtr Evict(Env*, const char*) { return tag_; }

  explicit Float(float fl) : Type() {
    uint64_t buf = 0;

    std::memcpy(reinterpret_cast<char*>(&buf) + 3, reinterpret_cast<char*>(&fl),
                sizeof(float));
    tag_ = MakeImmediate(buf, 0, IMMEDIATE_CLASS::FLOAT);
  }
};

} /* namespace libmu */

#endif /* _LIBMU_TYPES_FLOAT_H_ */
