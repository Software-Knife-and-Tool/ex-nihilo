/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
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
#include "libmu/types/exception.h"
#include "libmu/types/fixnum.h"

namespace libmu {
namespace mu {

using Frame = Env::Frame;

/** * mu function (char? object) => bool */
void IsChar(Frame* fp) { fp->value = Type::BoolOf(Char::IsType(fp->argv[0])); }

/** * mu function (char object) => chat **/
void CharCoerce(Frame* fp) {
  auto ch = fp->argv[0];

  if (Char::IsType(ch))
    fp->value = ch;
  else if (Fixnum::IsType(ch))
    fp->value = Char(Fixnum::Uint64Of(ch)).tag_;
  else
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "cannot be coerced to char (char)", ch);
}

} /* namespace mu */
} /* namespace libmu */
