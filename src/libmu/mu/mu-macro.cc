/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu-macro.cc: library macros
 **
 **/
#include <cassert>
#include <functional>
#include <vector>

#include "libmu/core.h"
#include "libmu/env.h"
#include "libmu/macro.h"
#include "libmu/type.h"

#include "libmu/types/condition.h"
#include "libmu/types/cons.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/float.h"
#include "libmu/types/function.h"
#include "libmu/types/stream.h"
#include "libmu/types/struct.h"
#include "libmu/types/symbol.h"

namespace libmu {
namespace mu {

using Condition = core::Condition;
using Macro = core::Macro;
using Frame = core::Env::Frame;
using Type = core::Type;

/** * (macro-function symbol) => function **/
void MacroFunction(Frame* fp) {
  auto macsym = fp->argv[0];

  if (!core::Symbol::IsType(macsym))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "macro-function", macsym);

  fp->value = Macro::MacroFunction(fp->env, macsym);
}

/** * (macroexpand form) => object **/
void MacroExpand(Frame* fp) {
  fp->value = Macro::MacroExpand(fp->env, fp->argv[0]);
}

} /* namespace mu */
} /* namespace libmu */
