/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  macro.h: library macros
 **
 **/
#if !defined(LIBMU_MACRO_H_)
#define LIBMU_MACRO_H_

#include <cassert>
#include <functional>
#include <map>
#include <vector>

#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/heap/heap.h"

#include "libmu/types/function.h"

namespace libmu {
namespace core {

/** * macro class type **/
class Macro : public Type {
 private:
  typedef struct {
    Tag func;
  } HeapLayout;

  HeapLayout macro_;

 public: /* Tag */
  static constexpr bool IsType(Tag ptr) {
    return IsExtended(ptr) && Heap::SysClass(ptr) == SYS_CLASS::MACRO;
  }

  static Tag func(Tag ptr) {
    assert(IsType(ptr));

    return Untag<HeapLayout>(ptr)->func;
  }

  static auto MacroExpand(Env*, Tag) -> Tag;
  static auto MacroFunction(Env*, Tag) -> Tag;

  static auto GcMark(Env*, Tag) -> void;
  static auto Print(Env*, Tag, Tag, bool) -> void;
  static auto ViewOf(Env*, Tag) -> Tag;

 public: /* type model */
  auto Evict(Env* env) -> Tag {
    auto sp = env->heap_alloc<HeapLayout>(sizeof(HeapLayout), SYS_CLASS::MACRO);

    *sp = macro_;
    sp->func = Env::Evict(env, sp->func);

    tag_ = Entag(sp, TAG::EXTEND);

    return tag_;
  }

 public: /* object */
  static auto EvictTag(Env* env, Tag macro) -> Tag {
    assert(IsType(macro));
    assert(!Env::IsEvicted(env, macro));

    // printf("EvictTag: macro\n");
    auto sp = env->heap_alloc<HeapLayout>(sizeof(HeapLayout), SYS_CLASS::MACRO);
    auto mp = Untag<HeapLayout>(macro);

    *sp = *mp;
    sp->func = Env::Evict(env, sp->func);

    return Entag(sp, TAG::EXTEND);
  }

  explicit Macro(Tag func) : Type() {
    assert(Function::IsType(func));

    macro_.func = func;

    tag_ = Entag(reinterpret_cast<void*>(&macro_), TAG::EXTEND);
  }
}; /* class Macro */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_MACRO_H_ */
