/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  macro.h: library macros
 **
 **/
#if !defined(_LIBMU_TYPES_MACRO_H_)
#define _LIBMU_TYPES_MACRO_H_

#include <cassert>
#include <functional>
#include <map>
#include <vector>

#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/heap/heap.h"

#include "libmu/types/function.h"

namespace libmu {

/** * macro class type **/
class Macro : public Type {
 private:
  typedef struct {
    TagPtr func;
  } Layout;

  Layout macro_;

 public: /* TagPtr */
  static constexpr bool IsType(TagPtr ptr) {
    return IsExtended(ptr) && Heap::SysClass(ptr) == SYS_CLASS::MACRO;
  }

  static TagPtr func(TagPtr ptr) {
    assert(IsType(ptr));

    return Untag<Layout>(ptr)->func;
  }

  static TagPtr MacroExpand(Env*, TagPtr);
  static TagPtr MacroFunction(Env*, TagPtr);

  static void GcMark(Env*, TagPtr);
  static TagPtr ViewOf(Env*, TagPtr);

 public: /* object model */
  TagPtr Evict(Env* env, const char* src) {
    auto sp = env->heap_alloc<Layout>(sizeof(Layout), SYS_CLASS::MACRO, src);

    *sp = macro_;
    tag_ = Entag(sp, TAG::EXTEND);

    return tag_;
  }

  explicit Macro(TagPtr func) : Type() {
    assert(Function::IsType(func));

    macro_.func = func;
    
    tag_ = Entag(reinterpret_cast<void*>(&macro_), TAG::EXTEND);
  }
}; /* class Macro */

} /* namespace libmu */

#endif /* _LIBMU_TYPES_MACRO_H_ */