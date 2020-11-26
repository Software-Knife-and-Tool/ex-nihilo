/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  readtable.h: kernel readtable
 **
 **/

#if !defined(_LIBMU_READTABLE_H_)
#define _LIBMU_READTABLE_H_

#include <cassert>
#include <map>

#include "libmu/type.h"

#include "libmu/types/char.h"

namespace libmu {

using TagPtr = Type::TagPtr;
using IMMEDIATE_CLASS = Type::IMMEDIATE_CLASS;

/** * readtable types **/
enum class SYNTAX_TYPE {
  CONSTITUENT,
  ILLEGAL,
  WSPACE,
  MACRO,
  TMACRO,
  ESCAPE,
  MESCAPE
};

/** * syntax characters **/
enum class SYNTAX_CHAR : uint64_t {
  AT = Type::ToUint64(Type::MakeImmediate('@', 1, IMMEDIATE_CLASS::CHAR)),
  BACKQUOTE =
      Type::ToUint64(Type::MakeImmediate('`', 0, IMMEDIATE_CLASS::CHAR)),
  BACKSLASH =
      Type::ToUint64(Type::MakeImmediate('\\', 0, IMMEDIATE_CLASS::CHAR)),
  COLON = Type::ToUint64(Type::MakeImmediate(':', 0, IMMEDIATE_CLASS::CHAR)),
  COMMA = Type::ToUint64(Type::MakeImmediate(',', 0, IMMEDIATE_CLASS::CHAR)),
  COMMENT = Type::ToUint64(Type::MakeImmediate(';', 0, IMMEDIATE_CLASS::CHAR)),
  CPAREN = Type::ToUint64(Type::MakeImmediate(')', 0, IMMEDIATE_CLASS::CHAR)),
  DOT = Type::ToUint64(Type::MakeImmediate('.', 0, IMMEDIATE_CLASS::CHAR)),
  OPAREN = Type::ToUint64(Type::MakeImmediate('(', 0, IMMEDIATE_CLASS::CHAR)),
  QUOTE = Type::ToUint64(Type::MakeImmediate('\'', 0, IMMEDIATE_CLASS::CHAR)),
  SHARP = Type::ToUint64(Type::MakeImmediate('#', 0, IMMEDIATE_CLASS::CHAR)),
  STRING = Type::ToUint64(Type::MakeImmediate('"', 0, IMMEDIATE_CLASS::CHAR)),
  UNBOUND = Type::ToUint64(Type::MakeImmediate(0, 0, IMMEDIATE_CLASS::CHAR)),
  VBAR = Type::ToUint64(Type::MakeImmediate('|', 0, IMMEDIATE_CLASS::CHAR))
};

bool SyntaxEq(TagPtr, SYNTAX_CHAR);

SYNTAX_TYPE MapSyntaxType(TagPtr);
SYNTAX_CHAR MapSyntaxChar(TagPtr);

} /* namespace libmu */

#endif /* _LIBMU_READTABLE_H_ */
