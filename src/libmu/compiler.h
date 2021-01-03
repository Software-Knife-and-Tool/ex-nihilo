/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  compiler.h: library form compiler
 **
 **/
#if !defined(LIBMU_COMPILER_H_)
#define LIBMU_COMPILER_H_

#include <cassert>

#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/types/cons.h"

namespace libmu {
namespace core {

Tag Compile(Env*, Tag);
bool IsSpecOp(Tag);

constexpr auto lexicals(Tag lambda) {
  assert(Cons::IsList(lambda));

  return Cons::car(lambda);
}

constexpr auto restsym(Tag lambda) {
  assert(Cons::IsList(lambda));

  return Cons::cdr(lambda);
}

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_COMPILER_H_ */
