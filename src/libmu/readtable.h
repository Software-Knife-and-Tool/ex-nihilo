/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  readtable.h: library readtable
 **
 **/

#if !defined(LIBMU_READTABLE_H_)
#define LIBMU_READTABLE_H_

#include <cassert>
#include <map>

#include "libmu/type.h"

#include "libmu/types/char.h"

namespace libmu {
namespace core {

using Tag = Type::Tag;
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

bool SyntaxEq(Tag, SYNTAX_CHAR);

SYNTAX_TYPE MapSyntaxType(Tag);
SYNTAX_CHAR MapSyntaxChar(Tag);

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_READTABLE_H_ */
