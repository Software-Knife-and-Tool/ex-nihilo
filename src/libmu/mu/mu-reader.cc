/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu-reader.cc: library reader
 **
 **/
#include <cassert>
#include <limits>
#include <map>

#include "libmu/env.h"
#include "libmu/print.h"
#include "libmu/read.h"
#include "libmu/readtable.h"
#include "libmu/type.h"

#include "libmu/types/char.h"
#include "libmu/types/cons.h"
#include "libmu/types/exception.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/float.h"
#include "libmu/types/function.h"
#include "libmu/types/namespace.h"
#include "libmu/types/stream.h"
#include "libmu/types/string.h"
#include "libmu/types/struct.h"
#include "libmu/types/symbol.h"

namespace libmu {
namespace mu {

using Exception = core::Exception;
using Frame = core::Env::Frame;
using Stream = core::Stream;
using Type = core::Type;

/** * mu function (read stream) => object **/
void Read(Frame* fp) {
  auto stream = Stream::StreamDesignator(fp->env, fp->argv[0]);

  if (!Stream::IsType(stream))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "read",
                     stream);

  if (Stream::IsEof(stream))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::END_OF_FILE, "read",
                     stream);

  fp->value = core::Read(fp->env, stream);
}

/** * mu function (set-macro-character char function) => object **/
void SetMacroChar(Frame* fp) {
  auto macro_char = fp->argv[0];
  auto reader = fp->argv[1];

  if (!core::Char::IsType(macro_char))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "(set-macro-character)", macro_char);

  if (!core::Function::IsType(reader))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "(set-macro-character)", reader);

  fp->env->readtable_[macro_char] = reader;
  fp->value = Type::T;
}

} /* namespace mu */
} /* namespace libmu */
