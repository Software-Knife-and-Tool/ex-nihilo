/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu-char.cc: library char functions
 **
 **/
#include <cassert>

#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/types/char.h"
#include "libmu/types/condition.h"
#include "libmu/types/fixnum.h"

namespace libmu {
namespace mu {

using Char = core::Char;
using Condition = core::Condition;
using Frame = core::Env::Frame;
using Type = core::Type;

/** * mu function (char? object) => bool */
void IsChar(Frame* fp) { fp->value = Type::Bool(Char::IsType(fp->argv[0])); }

} /* namespace mu */
} /* namespace libmu */
