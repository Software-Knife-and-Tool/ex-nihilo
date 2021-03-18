/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  char.cc: libmu char object
 **
 **/
#include "libmu/types/char.h"

#include <cassert>

#include "libmu/core.h"
#include "libmu/type.h"

#include "libmu/types/condition.h"
#include "libmu/types/stream.h"

namespace libmu {
namespace core {

/** * view of char object **/
auto Char::ViewOf(Env* env, Tag ch) -> Tag {
  assert(IsType(ch));

  auto view = std::vector<Tag>{Symbol::Keyword("char"), ch,
                               Fixnum(ToUint64(ch) >> 3).tag_,
                               Fixnum(Uint8Of(ch)).tag_};

  return Vector(env, view).tag_;
}

/** * print char object to stream **/
auto Char::Print(Env* env, Tag ch, Tag stream, bool esc) -> void {
  assert(IsType(ch));
  assert(Stream::IsType(stream));

  if (esc) {
    core::PrintStdString(env, "#\\", stream, false);
    switch (Char::Uint8Of(ch)) {
      case 0x0a:
        core::PrintStdString(env, "newline", stream, false);
        break;
      case 0x0d:
        core::PrintStdString(env, "return", stream, false);
        break;
      case ' ':
        core::PrintStdString(env, "space", stream, false);
        break;
      case '\t':
        core::PrintStdString(env, "tab", stream, false);
        break;
      default:
        Stream::WriteByte(Fixnum(Uint8Of(ch)).tag_, stream);
        break;
    }
  } else {
    Stream::WriteByte(Fixnum(Uint8Of(ch)).tag_, stream);
  }
}

/** * read char from stream **/
auto Char::Read(Env* env, Tag stream) -> Tag {
  static const std::map<std::string, int> kCharLit{
      {"newline", 0x0a}, {"space", 0x20},   {"rubout", 0x7f},
      {"page", 0x0c},    {"tab", 0x09},     {"backspace", 0x08},
      {"return", 0x0d},  {"linefeed", 0x0a}};

  std::string buffer;
  Tag value;

  auto ch = Stream::ReadByte(env, stream);

  if (Type::Null(ch))
    Condition::Raise(env, Condition::CONDITION_CLASS::READER_ERROR,
                     "eof in #\\", Type::NIL);

  if (core::MapSyntaxType(ch) == core::SYNTAX_TYPE::WSPACE)
    Condition::Raise(env, Condition::CONDITION_CLASS::READER_ERROR,
                     "malformed character literal", Type::NIL);

  buffer.push_back(Fixnum::Int64Of(ch));

  for (;;) {
    ch = Stream::ReadByte(env, stream);
    if (Type::Null(ch) ||
        core::MapSyntaxType(ch) != core::SYNTAX_TYPE::CONSTITUENT)
      break;
    buffer.push_back(Fixnum::Int64Of(ch));
  }

  if (Type::Null(ch))
    Condition::Raise(env, Condition::CONDITION_CLASS::READER_ERROR,
                     "eof in #\\", Type::NIL);

  Stream::UnReadByte(ch, stream);

  if (buffer.length() == 1)
    value = Char(buffer[0]).tag_;
  else if (kCharLit.count(buffer))
    value = Char(kCharLit.at(buffer)).tag_;
  else
    Condition::Raise(env, Condition::CONDITION_CLASS::READER_ERROR,
                     "unmapped char", String(env, buffer).tag_);

  return value;
}

} /* namespace core */
} /* namespace libmu */
