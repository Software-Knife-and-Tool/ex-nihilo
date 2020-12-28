/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  struct.h: library structs
 **
 **/
#if !defined(_LIBMU_TYPES_STRUCT_H_)
#define _LIBMU_TYPES_STRUCT_H_

#include <cassert>

#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/heap/heap.h"
#include "libmu/types/cons.h"

namespace libmu {
namespace core {

/** * struct type class **/
class Struct : public Type {
 private:
  typedef struct {
    TagPtr stype; /* keyword */
    TagPtr slots; /* slot values alist (slot-name . value)) */
  } Layout;

  Layout struct_;

 public: /* TagPtr */
  static constexpr bool IsType(TagPtr ptr) {
    return IsExtended(ptr) && Heap::SysClass(ptr) == SYS_CLASS::STRUCT;
  }

  static TagPtr stype(TagPtr str) { return Untag<Layout>(str)->stype; }
  static TagPtr slots(TagPtr str) { return Untag<Layout>(str)->slots; }

  static void GcMark(Env* env, TagPtr ptr) {
    assert(IsType(ptr));

    if (!env->heap_->IsGcMarked(ptr)) {
      env->heap_->GcMark(ptr);
      env->GcMark(env, slots(ptr));
      env->GcMark(env, stype(ptr));
    }
  }

  /** * view of struct object **/
  static TagPtr ViewOf(Env* env, TagPtr strct) {
    assert(IsType(strct));

    auto view = std::vector<TagPtr>{Symbol::Keyword("struct"), strct,
                                    Fixnum(ToUint64(strct) >> 3).tag_,
                                    stype(strct), slots(strct)};

    return Vector(env, view).tag_;
  }

 public: /* object model */
  TagPtr Evict(Env* env) {
    auto sp = env->heap_alloc<Layout>(sizeof(Layout), SYS_CLASS::STRUCT);

    *sp = struct_;
    tag_ = Entag(sp, TAG::EXTEND);

    return tag_;
  }

  explicit Struct(TagPtr name, TagPtr slots) {
    assert(Symbol::IsKeyword(name));
    assert(Cons::IsList(slots));

    struct_.stype = name;
    struct_.slots = slots;

    tag_ = Entag(reinterpret_cast<void*>(&struct_), TAG::EXTEND);
  }

}; /* class Struct */

} /* namespace core */
} /* namespace libmu */

#endif /* _LIBMU_TYPES_STRUCT_H_ */
