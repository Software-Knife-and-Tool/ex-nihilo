/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  struct.h: library structs
 **
 **/
#if !defined(LIBMU_TYPES_STRUCT_H_)
#define LIBMU_TYPES_STRUCT_H_

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
    Tag stype; /* keyword */
    Tag slots; /* slot values alist (slot-name . value)) */
  } HeapLayout;

  HeapLayout struct_;

 public: /* Tag */
  static constexpr auto IsType(Tag ptr) -> bool {
    return IsExtended(ptr) && Heap::SysClass(ptr) == SYS_CLASS::STRUCT;
  }

  static auto stype(Tag str) -> Tag { return Untag<HeapLayout>(str)->stype; }
  static auto slots(Tag str) -> Tag { return Untag<HeapLayout>(str)->slots; }

  static auto GcMark(Env* env, Tag ptr) -> void {
    assert(IsType(ptr));

    if (!env->heap_->IsGcMarked(ptr)) {
      env->heap_->GcMark(ptr);
      env->GcMark(env, slots(ptr));
      env->GcMark(env, stype(ptr));
    }
  }

  /** * view of struct object **/
  static auto ViewOf(Env* env, Tag strct) -> Tag {
    assert(IsType(strct));

    auto view = std::vector<Tag>{Symbol::Keyword("struct"), strct,
                                 Fixnum(ToUint64(strct) >> 3).tag_,
                                 stype(strct), slots(strct)};

    return Vector(env, view).tag_;
  }

 public: /* type model */
  auto Evict(Env* env) -> Tag {
    auto sp =
        env->heap_alloc<HeapLayout>(sizeof(HeapLayout), SYS_CLASS::STRUCT);

    *sp = struct_;
    tag_ = Entag(sp, TAG::EXTEND);

    return tag_;
  }

 public: /* object */
  explicit Struct(Tag name, Tag slots) {
    assert(Symbol::IsKeyword(name));
    assert(Cons::IsList(slots));

    struct_.stype = name;
    struct_.slots = slots;

    tag_ = Entag(reinterpret_cast<void*>(&struct_), TAG::EXTEND);
  }

}; /* class Struct */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TYPES_STRUCT_H_ */
