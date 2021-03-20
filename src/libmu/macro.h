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

  static Tag MacroExpand(Env*, Tag);
  static Tag MacroFunction(Env*, Tag);
  static void Print(Env*, Tag, Tag, bool);
  static void GcMark(Env*, Tag);
  static Tag ViewOf(Env*, Tag);

 public: /* object model */
  auto Evict(Env* env) -> Tag {
    auto sp = env->heap_alloc<HeapLayout>(sizeof(HeapLayout), SYS_CLASS::MACRO);

    *sp = macro_;
    tag_ = Entag(sp, TAG::EXTEND);

    return tag_;
  }

  static auto EvictTag(Env* env, Tag macro) -> void {
    assert(IsType(macro));
    assert(!Env::IsEvicted(env, macro));

    printf("not evicitng macro\n");
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
