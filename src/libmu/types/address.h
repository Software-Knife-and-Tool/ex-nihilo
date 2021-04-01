/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  address.h: library address object
 **
 **/
#if !defined(LIBMU_TYPES_ADDRESS_H_)
#define LIBMU_TYPES_ADDRESS_H_

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
 public: /* tag */
  static constexpr auto IsType(Tag ptr) -> bool {
    return TagOf(ptr) == TAG::ADDRESS;
  }

  static auto ViewOf(Tag addr) {
    assert(IsType(addr));

    auto view = std::vector<Tag>{
        Symbol::Keyword("address"),
        addr,
        Fixnum(ToUint64(addr)).tag_,
    };

    return Vector<Tag>(view).tag_;
  }

 public: /* type model */
  auto Evict(Env*) -> Tag { return tag_; }

 public: /* object */
  explicit Address(void* caddr) : Type() {
    tag_ = Type::Entag(caddr, TAG::ADDRESS);
  }

}; /* class Address */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TYPES_ADDRESS_H_ */
