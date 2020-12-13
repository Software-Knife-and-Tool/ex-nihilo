/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  address.h: library address object
 **
 **/
#if !defined(_LIBMU_TYPES_ADDRESS_H_)
#define _LIBMU_TYPES_ADDRESS_H_

#include <cassert>
#include <functional>
#include <map>
#include <vector>

#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/heap/heap.h"

#include "libmu/types/fixnum.h"
#include "libmu/types/symbol.h"

namespace libmu {
namespace core {

/** * address class type **/
class Address : public Type {
 public: /* TagPtr */
  static constexpr bool IsType(TagPtr ptr) {
    return TagOf(ptr) == TAG::ADDRESS;
  }

  static TagPtr ViewOf(Env* env, TagPtr addr) {
    assert(IsType(addr));

    auto view = std::vector<TagPtr>{
        Symbol::Keyword("address"),
        addr,
        Fixnum(ToUint64(addr)).tag_,
    };

    return Vector(env, view).tag_;
  }

 public: /* object model */
  TagPtr Evict(Env*, const char*) { return tag_; }

  explicit Address(void* caddr) : Type() {
    tag_ = Type::Entag(caddr, TAG::ADDRESS);
  }

}; /* class Address */

} /* namespace core */
} /* namespace libmu */

#endif /* _LIBMU_TYPES_ADDRESS_H_ */
