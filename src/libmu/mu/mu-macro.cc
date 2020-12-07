/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
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

#include "libmu/env.h"
#include "libmu/print.h"
#include "libmu/type.h"

#include "libmu/types/cons.h"
#include "libmu/types/exception.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/float.h"
#include "libmu/types/function.h"
#include "libmu/types/macro.h"
#include "libmu/types/stream.h"
#include "libmu/types/struct.h"
#include "libmu/types/symbol.h"

namespace libmu {
namespace mu {

/** * (macro-function symbol) => function **/
void MacroFunction(Env::Frame* fp) {
  auto macsym = fp->argv[0];

  if (!Symbol::IsType(macsym))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "macro-function", macsym);

  fp->value = Macro::MacroFunction(fp->env, macsym);
}

/** * (macroexpand form) => object **/
void MacroExpand(Env::Frame* fp) {
  fp->value = Macro::MacroExpand(fp->env, fp->argv[0]);
}

} /* namespace mu */
} /* namespace libmu */
