/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  print.cc: library printer
 **
 **/
#include <bitset>
#include <cassert>
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>
#include <utility>

#include "libmu/env.h"
#include "libmu/print.h"
#include "libmu/type.h"

#include "libmu/types/macro.h"
#include "libmu/types/char.h"
#include "libmu/types/cons.h"
#include "libmu/types/exception.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/float.h"
#include "libmu/types/function.h"
#include "libmu/types/stream.h"
#include "libmu/types/string.h"
#include "libmu/types/struct.h"
#include "libmu/types/symbol.h"
#include "libmu/types/vector.h"

namespace libmu {

using TagPtr = Type::TagPtr;
using SYS_CLASS = Type::SYS_CLASS;

/** * print std::string to stream **/
void PrintStdString(Env* env, const std::string& str, TagPtr strm, bool esc) {
  auto stream = Stream::StreamDesignator(env, strm);

  auto dq = Char('"').tag_;

  if (esc)
    Print(env, dq, stream, false);

  for (auto ch : str)
    Print(env, Char(ch).tag_, stream, false);

  if (esc)
    Print(env, dq, stream, false);
}

/** * print object in broket syntax to stream **/
void PrintUnreadable(Env* env, TagPtr object, TagPtr str) {
  auto stream = Stream::StreamDesignator(env, str);

  auto type = String::StdStringOf(
      Symbol::name(Type::MapClassSymbol(Type::TypeOf(object))));
  std::stringstream hexs;

  hexs << std::hex << Type::to_underlying(object);
  PrintStdString(env, "#<:" + type + " 0x" + hexs.str() + ";>", stream, false);
}

/** * print object to stream **/
void Print(Env* env, TagPtr object, TagPtr str, bool esc) {
  auto stream = Stream::StreamDesignator(env, str);

  static const std::map<SYS_CLASS,
                        std::function<void(Env*, TagPtr, TagPtr, bool)>>
      kPrinMap{{SYS_CLASS::CHAR, Char::PrintChar},
               {SYS_CLASS::CONS, Cons::PrintCons},
               {SYS_CLASS::FIXNUM, Fixnum::PrintFixnum},
               {SYS_CLASS::FLOAT, Float::PrintFloat},
               {SYS_CLASS::FUNCTION, Function::PrintFunction},
               {SYS_CLASS::STRING, String::PrintString},
               {SYS_CLASS::SYMBOL, Symbol::PrintSymbol},
               {SYS_CLASS::NULLT, Symbol::PrintSymbol},
               {SYS_CLASS::VECTOR, Vector::PrintVector}};

  auto printh = kPrinMap.count(Type::TypeOf(object)) != 0;

  if (esc && !printh)
    Exception::Raise(env, Exception::EXCEPT_CLASS::PRINT_NOT_READABLE,
                     "printing unreadable object", Type::NIL);

  if (printh)
    kPrinMap.at(Type::TypeOf(object))(env, object, stream, esc);
  else
    PrintUnreadable(env, object, stream);
}

/** * print newline to stream **/
void Terpri(Env* env, TagPtr stream) {
  Print(env, Char('\n').Evict(env, ""), Stream::StreamDesignator(env, stream),
        false);
}

} /* namespace libmu */
