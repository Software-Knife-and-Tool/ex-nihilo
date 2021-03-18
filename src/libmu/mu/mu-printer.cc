/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu-printer.cc: library print functions
 **
 **/
#include <bitset>
#include <cassert>
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>
#include <utility>

#include "libmu/core.h"
#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/types/char.h"
#include "libmu/types/condition.h"
#include "libmu/types/cons.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/float.h"
#include "libmu/types/function.h"
#include "libmu/types/stream.h"
#include "libmu/types/string.h"
#include "libmu/types/struct.h"
#include "libmu/types/symbol.h"

namespace libmu {
namespace mu {

using Condition = core::Condition;
using Frame = core::Env::Frame;
using Type = core::Type;

/** * (print object stream) => object **/
void PrintEscape(Frame* fp) {
  auto obj = fp->argv[0];
  auto stream = fp->argv[1];
  auto escape = fp->argv[2];

  if (!core::Stream::IsStreamDesignator(stream))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "print",
                     stream);

  core::Print(fp->env, obj, stream, !Type::Null(escape));

  fp->value = obj;
}

/** * (terpri stream) => :nil **/
void Terpri(Frame* fp) {
  auto stream = fp->argv[0];

  if (!core::Stream::IsStreamDesignator(stream))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "terpri",
                     stream);

  core::Terpri(fp->env, stream);
  fp->value = Type::NIL;
}

} /* namespace mu */
} /* namespace libmu */
