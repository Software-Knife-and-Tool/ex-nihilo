/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  string.cc: library string object
 **
 **/
#include "libmu/types/string.h"

#include <cassert>

#include "libmu/env.h"
#include "libmu/print.h"
#include "libmu/type.h"

#include "libmu/types/char.h"
#include "libmu/types/exception.h"
#include "libmu/types/stream.h"
#include "libmu/types/vector.h"

namespace libmu {

/** * view of string object **/
Type::TagPtr String::ViewOf(Env* env, TagPtr string) {
  assert(IsType(string));
  
  auto view = std::vector<TagPtr>{
    Symbol::Keyword("string"),
    string,
    Fixnum(ToUint64(string) >> 3).tag_,
  };
    
  return Vector(env, view).tag_;
}

void String::Print(Env* env, TagPtr string, TagPtr stream, bool esc) {
  assert(String::IsType(string));
  assert(Stream::IsType(stream));

  if (esc) PrintStdString(env, "\"", stream, false);

  vector_iter<char> iter(string);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    Print(env, Char(*it).tag_, stream, false);

  if (esc) PrintStdString(env, "\"", stream, false);
}

/** * read string **/
TagPtr String::Read(Env* env, TagPtr stream) {
  assert(Stream::IsType(stream));

  std::string str;

  auto ch = Stream::ReadByte(env, stream);
  for (size_t len = 0; MapSyntaxChar(ch) != SYNTAX_CHAR::STRING; len++) {
    if (len >= Vector::MAX_LENGTH)
      Exception::Raise(env, Exception::EXCEPT_CLASS::PARSE_ERROR,
                       "string exceeds maximum limit", Type::NIL);

    if (MapSyntaxChar(ch) == SYNTAX_CHAR::BACKSLASH) {
      ch = Stream::ReadByte(env, stream);
      if (Type::Null(ch))
        Exception::Raise(env, Exception::EXCEPT_CLASS::END_OF_FILE,
                         "EOF in string", stream);
    }

    str.push_back(Fixnum::Uint64Of(ch));
    ch = Stream::ReadByte(env, stream);

    if (Type::Null(ch))
      Exception::Raise(env, Exception::EXCEPT_CLASS::END_OF_FILE, "EOF in string",
                       stream);
  }

  return String(env, str).tag_;
}

} /* namespace libmu */
