/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  read.cc: library reader
 **
 **/
#include "libmu/core.h"

#include <cassert>
#include <limits>
#include <map>

#include "libmu/compiler.h"
#include "libmu/core.h"
#include "libmu/env.h"
#include "libmu/macro.h"
#include "libmu/readtable.h"
#include "libmu/type.h"

#include "libmu/types/char.h"
#include "libmu/types/cons.h"

#include "libmu/types/condition.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/float.h"
#include "libmu/types/function.h"
#include "libmu/types/namespace.h"
#include "libmu/types/stream.h"
#include "libmu/types/string.h"
#include "libmu/types/struct.h"
#include "libmu/types/symbol.h"
#include "libmu/types/vector.h"

namespace libmu {
namespace core {
namespace {

/** * read #<:type fixnum attr-list> syntax **/
auto ReadBroketSyntax(Env* env, Tag stream) -> Tag {
  assert(Stream::IsType(stream));

  auto type = ReadForm(env, stream);
  auto tagptr = ReadForm(env, stream);

  /* think: this is the attribute list. Do something with it? */
  /* the attribute list is there to keep the reader from bumping into the > */
  (void)ReadForm(env, stream);

  auto bracket = Stream::ReadByte(env, stream);
  if (Fixnum::Uint64Of(bracket) != '>')
    Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "broket syntax (terminal)", bracket);

  auto sysclass = Type::MapSymbolClass(type);
  auto tagptr_bits = Fixnum::Uint64Of(tagptr);

  if (!Fixnum::IsType(tagptr))
    Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "broket syntax (type)", tagptr);

  switch (sysclass) {
    case SYS_CLASS::CONDITION:
    case SYS_CLASS::FUNCTION:
    case SYS_CLASS::MACRO:
    case SYS_CLASS::NAMESPACE:
    case SYS_CLASS::STREAM:
    case SYS_CLASS::STRUCT:
    default:
      Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR,
                       "broket syntax (unimplemented)", type);
      break;
  }

  return Type::FromUint64(tagptr_bits);
}

/** * read atom **/
auto Atom(Env* env, Tag stream) -> std::string {
  assert(Stream::IsType(stream));

  std::string string;

  auto ch = Stream::ReadByte(env, stream);
  for (size_t len = 0;
       !Type::Null(ch) && MapSyntaxType(ch) == SYNTAX_TYPE::CONSTITUENT;
       len++) {
    if (len >= Vector::MAX_LENGTH)
      Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                       "atom exceeds maximum size (read-atom)", Type::NIL);

    string.push_back(Fixnum::Int64Of(ch));
    ch = Stream::ReadByte(env, stream);
  }

  if (!Type::Null(ch)) Stream::UnReadByte(ch, stream);

  if (string.size() == 0)
    Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                     "naked atom syntax (read-atom)", Type::NIL);

  return string;
}

/** * read radixed fixnum from stream **/
auto RadixFixnum(Env* env, int radix, Tag stream) -> Tag {
  assert(Stream::IsType(stream));

  std::string str = Atom(env, stream);
  auto number = Type::NIL;

  try {
    size_t n;
    uint64_t fxval = std::stoull(str, &n, radix);

    if (n == str.length()) {
      if (((fxval >> 62) & 1) ^ ((fxval >> 63) & 1))
        Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                         "parse-number", String(env, str).tag_);
      number = Fixnum(fxval).tag_;
    }
  } catch (std::invalid_argument& ex) {
    Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                     "parse-number", String(env, str).tag_);
  } catch (std::out_of_range& ex) {
    Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                     "parse-number", String(env, str).tag_);
  } catch (std::exception& ex) {
    Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                     "parse-number", String(env, str).tag_);
  }

  return number;
}

/** * parse a numeric std::string **/
auto Number(Env* env, const std::string& str) -> Tag {
  auto number = Type::NIL;

  try {
    size_t n;
    uint64_t fxval = std::stoull(str, &n, 0);

    if (n == str.length()) {
      if (((fxval >> 62) & 1) ^ ((fxval >> 63) & 1))
        Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                         "parse-number:fixnum", String(env, str).tag_);

      number = Fixnum(fxval).tag_;
    } else {
      auto flval = std::stof(str, &n);

      if (n == str.length()) return Float(flval).tag_;
    }
  } catch (std::invalid_argument& ex) {
    return Type::NIL;
  } catch (std::out_of_range& ex) {
    return Type::NIL;
  } catch (std::exception& ex) {
    Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                     "malformed float", String(env, str).tag_);
  }

  return number;
}

} /* anonymous namespace */

/** * absorb whitespace until eof **/
auto ReadWSUntilEof(Env* env, Tag stream) -> bool {
  assert(Stream::IsType(stream));

  Tag ch;

  do {
    ch = Stream::ReadByte(env, stream);
    if (!Type::Null(ch) && MapSyntaxChar(ch) == SYNTAX_CHAR::COMMENT) {
      do {
        ch = Stream::ReadByte(env, stream);
      } while (!Type::Null(ch) && Fixnum::Int64Of(ch) != '\n');
    }
  } while (!Type::Null(ch) && MapSyntaxType(ch) == SYNTAX_TYPE::WSPACE);

  if (Type::Null(ch)) return false;

  Stream::UnReadByte(ch, stream);
  return true;
}

/** * read form **/
auto ReadForm(Env* env, Tag stream_designator) -> Tag {
  Tag rval;

  auto stream = Stream::StreamDesignator(env, stream_designator);

  if (!ReadWSUntilEof(env, stream)) return Type::NIL;

  auto ch = Stream::ReadByte(env, stream);

  /* macro character expander */
  auto chm = Char(Fixnum::Int64Of(ch)).tag_;
  if (env->readtable_.count(chm) > 0)
    return Function::Funcall(env, env->readtable_[chm],
                             std::vector<Tag>{stream, chm});

  switch (MapSyntaxChar(ch)) {
    case SYNTAX_CHAR::COMMENT:
      do {
        ch = Stream::ReadByte(env, stream);
      } while (!Type::Null(ch) && Fixnum::Int64Of(ch) != '\n');

      rval = Type::Null(ch) ? Type::NIL : ReadForm(env, stream);
      break;
    case SYNTAX_CHAR::OPAREN: /* list syntax */
      rval = Cons::Read(env, stream);
      break;
    case SYNTAX_CHAR::QUOTE: /* quote syntax */
      rval = ReadForm(env, stream);
      if (Symbol::IsType(rval) || Cons::IsType(rval))
        rval =
            Cons::List(env, std::vector<Tag>{Symbol::Keyword("quote"), rval});
      break;
    case SYNTAX_CHAR::STRING: /* string syntax */
      rval = String::Read(env, stream);
      break;
    case SYNTAX_CHAR::SHARP: /* sharp syntax */
      ch = Stream::ReadByte(env, stream);
      if (Type::Null(ch))
        Condition::Raise(env, Condition::CONDITION_CLASS::END_OF_FILE, "read",
                         stream);
      else if (Fixnum::Int64Of(ch) == '<')
        rval = ReadBroketSyntax(env, stream);
      else if (Fixnum::Int64Of(ch) == 'x')
        rval = RadixFixnum(env, 16, stream);
      else if (Fixnum::Int64Of(ch) == 'd')
        rval = RadixFixnum(env, 10, stream);
      else if (Fixnum::Int64Of(ch) == 'o')
        rval = RadixFixnum(env, 8, stream);
      else
        switch (MapSyntaxChar(ch)) {
          case SYNTAX_CHAR::BACKSLASH: /* char syntax */
            rval = Char::Read(env, stream);
            break;
          case SYNTAX_CHAR::OPAREN: /* vector syntax */
            rval = Vector::Read(env, stream);
            break;
          case SYNTAX_CHAR::QUOTE: { /* closure syntax */
            auto fn = ReadForm(env, stream);
            if (Type::Null(ch))
              Condition::Raise(env, Condition::CONDITION_CLASS::END_OF_FILE,
                               "read", stream);
            rval = Cons::List(
                env, std::vector<Tag>{
                         Namespace::FindSymbol(env, env->mu_,
                                               String(env, "closure").tag_),
                         fn});
            break;
          }
          case SYNTAX_CHAR::COLON: { /* uninterned symbol */
            std::string atom = Atom(env, stream);
            rval = Number(env, atom);
            if (!Type::Null(rval))
              Condition::Raise(env, Condition::CONDITION_CLASS::READER_ERROR,
                               "uninterned symbol", String(env, atom).tag_);
            rval = Symbol::ParseSymbol(env, atom, false);
            break;
          }
          case SYNTAX_CHAR::DOT: /* read-time eval */
            rval = Eval(env, Compile(env, ReadForm(env, stream)));
            break;
          case SYNTAX_CHAR::VBAR: /* block comment */
            for (;;) {
              ch = Stream::ReadByte(env, stream);
              if (Type::Null(ch)) break;
              if (Fixnum::Int64Of(ch) == '|') {
                ch = Stream::ReadByte(env, stream);
                if (Type::Null(ch)) break;
                if (Fixnum::Int64Of(ch) == '#') break;
                Stream::UnReadByte(ch, stream);
              }
            }

            if (Type::Null(ch))
              Condition::Raise(env, Condition::CONDITION_CLASS::READER_ERROR,
                               "eof in block comment", ch);

            rval = ReadForm(env, stream);
            break;
          default:
            Condition::Raise(env, Condition::CONDITION_CLASS::READER_ERROR,
                             "# syntax", ch);
        }
      break;
    case SYNTAX_CHAR::CPAREN:
      Condition::Raise(env, Condition::CONDITION_CLASS::READER_ERROR,
                       "naked syntax", Char(')').tag_);
      break;
    default: { /* unadorned atom */
      Stream::UnReadByte(ch, stream);
      auto atom = Atom(env, stream);
      rval = Number(env, atom);
      if (Type::Null(rval)) rval = Symbol::ParseSymbol(env, atom, true);
      break;
    }
  }

  return rval;
}

/** * read **/
auto Read(Env* env, Tag stream_designator) -> Tag {
  auto stream = Stream::StreamDesignator(env, stream_designator);

  return Stream::IsFunction(stream)
             ? Function::Funcall(env, Stream::func(stream), std::vector<Tag>{})
             : ReadForm(env, Stream::StreamDesignator(env, stream));
}

} /* namespace core */
} /* namespace libmu */
