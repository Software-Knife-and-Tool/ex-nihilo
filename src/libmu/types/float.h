/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  float.h: library floats
 **
 **/
#if !defined(LIBMU_TYPES_FLOAT_H_)
#define LIBMU_TYPES_FLOAT_H_

#include <cassert>
#include <cstring>

#include "libmu/type.h"

namespace libmu {
namespace core {

/** * float type class **/
class Float : public Type {
 public: /* Tag */
  static constexpr bool IsType(Tag ptr) {
    return IsImmediate(ptr) && (ImmediateClass(ptr) == IMMEDIATE_CLASS::FLOAT);
  }

  static float FloatOf(Tag fl) {
    assert(IsType(fl));

    float buf = 0.0;

    std::memcpy(reinterpret_cast<char*>(&buf), reinterpret_cast<char*>(&fl) + 4,
                sizeof(float));
    return buf;
  }

  static float VSpecOf(Tag fl) { return FloatOf(fl); }
  static void Print(Env*, Tag, Tag, bool);
  static Tag ViewOf(Env*, Tag);

 public: /* object model */
  Tag Evict(Env*) { return tag_; }

  explicit Float(float fl) : Type() {
    uint64_t buf = 0;

    std::memcpy(reinterpret_cast<char*>(&buf) + 3, reinterpret_cast<char*>(&fl),
                sizeof(float));
    tag_ = MakeImmediate(buf, 0, IMMEDIATE_CLASS::FLOAT);
  }
};

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TYPES_FLOAT_H_ */
