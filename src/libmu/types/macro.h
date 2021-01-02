/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
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
namespace core {

/** * macro class type **/
class Macro : public Type {
 private:
  typedef struct {
    Tag func;
  } Layout;

  Layout macro_;

 public: /* Tag */
  static constexpr bool IsType(Tag ptr) {
    return IsExtended(ptr) && Heap::SysClass(ptr) == SYS_CLASS::MACRO;
  }

  static Tag func(Tag ptr) {
    assert(IsType(ptr));

    return Untag<Layout>(ptr)->func;
  }

  static Tag MacroExpand(Env*, Tag);
  static Tag MacroFunction(Env*, Tag);

  static void Print(Env* env, Tag macro, Tag str, bool) {
    auto fn = func(macro);

    assert(Function::IsType(fn));
    assert(Stream::IsType(str));

    auto stream = Stream::StreamDesignator(env, str);
    auto name = String::StdStringOf(Symbol::name(Function::name(fn)));

    std::stringstream hexs;

    hexs << std::hex << Type::to_underlying(fn);
    core::PrintStdString(env, "#<:macro #x" + hexs.str() + " (" + name + ")>",
                         stream, false);
  }

  static void GcMark(Env*, Tag);
  static Tag ViewOf(Env*, Tag);

 public: /* object model */
  Tag Evict(Env* env) {
    auto sp = env->heap_alloc<Layout>(sizeof(Layout), SYS_CLASS::MACRO);

    *sp = macro_;
    tag_ = Entag(sp, TAG::EXTEND);

    return tag_;
  }

  explicit Macro(Tag func) : Type() {
    assert(Function::IsType(func));

    macro_.func = func;

    tag_ = Entag(reinterpret_cast<void*>(&macro_), TAG::EXTEND);
  }
}; /* class Macro */

} /* namespace core */
} /* namespace libmu */

#endif /* _LIBMU_TYPES_MACRO_H_ */
