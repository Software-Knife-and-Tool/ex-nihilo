/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  eval.h: eval/apply
 **
 **/

#if !defined(_LIBMU_EVAL_H_)
#define _LIBMU_EVAL_H_

#include <cassert>
#include <cinttypes>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "libmu/env.h"
#include "libmu/type.h"

namespace libmu {

using TagPtr = Type::TagPtr;

TagPtr Apply(Env*, TagPtr, TagPtr);
TagPtr Eval(Env*, TagPtr);

} /* namespace libmu */

#endif /* _LIBMU_EVAL_H_ */
