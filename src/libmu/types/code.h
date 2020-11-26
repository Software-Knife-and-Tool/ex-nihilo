/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  code.h: library code object
 **
 **/
#if !defined(_LIBMU_TYPES_CODE_H_)
#define _LIBMU_TYPES_CODE_H_

#include <cassert>
#include <functional>
#include <map>
#include <vector>

#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/heap/heap.h"

namespace libmu {

/** * code class type **/
class Code : public Type {
 private:
  typedef struct {
    Env::FrameFn func;
  } Layout;

  Layout code_;

 public: /* TagPtr */
  static constexpr bool IsType(TagPtr ptr) {

    return IsExtended(ptr) && Heap::SysClass(ptr) == SYS_CLASS::CODE;
  }

  static Env::FrameFn func(TagPtr ptr) {
    assert(IsType(ptr));

    return Untag<Layout>(ptr)->func;
  }

  static void GcMark(Env* env, TagPtr code) {

    env->heap_->GcMark(code);
  }

  static void Call(TagPtr code, Env::Frame* fp) {

    (func(code))(fp);
  }

  static TagPtr ViewOf(Env* env, TagPtr code) {
    assert(IsType(code));
  
    auto view = std::vector<TagPtr>{
      Symbol::Keyword("code"),
      code,
      Fixnum(ToUint64(code) >> 3).tag_,
      // func(ns)
    };
    
    return Vector(env, view).tag_;
  }

 public: /* object model */
  TagPtr Evict(Env* env, const char* src) {
    auto sp = env->heap_alloc<Layout>(sizeof(Layout), SYS_CLASS::CODE, src);

    *sp = code_;

    return Entag(sp, TAG::EXTEND);
  }

  explicit Code(Env::FrameFn func) : Type() {
    code_.func = func;

    tag_ = Entag(reinterpret_cast<void*>(&code_), TAG::EXTEND);
  }

}; /* class Code */

} /* namespace libmu */

#endif /* _LIBMU_TYPES_CODE_H_ */
