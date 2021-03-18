/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
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

#include "libmu/core.h"
#include "libmu/env.h"
#include "libmu/macro.h"
#include "libmu/type.h"

#include "libmu/types/address.h"
#include "libmu/types/char.h"
#include "libmu/types/condition.h"
#include "libmu/types/cons.h"
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

/** * print std::string to stream **/
void PrintStdString(Env* env, const std::string& str, Tag strm, bool esc) {
  auto stream = Stream::StreamDesignator(env, strm);

  auto dq = Char('"').tag_;

  if (esc) Print(env, dq, stream, false);

  for (auto ch : str) Print(env, Char(ch).tag_, stream, false);

  if (esc) Print(env, dq, stream, false);
}

/** * print object in broket syntax to stream **/
void PrintAsBroket(Env* env, Tag object, Tag str) {
  auto stream = Stream::StreamDesignator(env, str);

  auto type = String::StdStringOf(
      Symbol::name(Type::MapClassSymbol(Type::TypeOf(object))));
  std::stringstream hexs;

  hexs << std::hex << Type::to_underlying(object);
  PrintStdString(env, "#<:" + type + " #x" + hexs.str() + "()>", stream, false);
}

/** * print object to stream **/
void Print(Env* env, Tag object, Tag str, bool esc) {
  auto stream = Stream::StreamDesignator(env, str);

  static const std::map<SYS_CLASS, std::function<void(Env*, Tag, Tag, bool)>>
      kPrinMap{{SYS_CLASS::CHAR, Char::Print},
               {SYS_CLASS::CONS, Cons::Print},
               {SYS_CLASS::FIXNUM, Fixnum::Print},
               {SYS_CLASS::FLOAT, Float::Print},
               {SYS_CLASS::FUNCTION, Function::Print},
               {SYS_CLASS::MACRO, Macro::Print},
               {SYS_CLASS::NAMESPACE, Namespace::Print},
               {SYS_CLASS::STRING, String::Print},
               {SYS_CLASS::SYMBOL, Symbol::Print},
               {SYS_CLASS::VECTOR, Vector::Print}};

  auto printh = kPrinMap.count(Type::TypeOf(object)) != 0;

  if (printh)
    kPrinMap.at(Type::TypeOf(object))(env, object, stream, esc);
  else
    PrintAsBroket(env, object, stream);
}

/** * print newline to stream **/
void Terpri(Env* env, Tag stream) {
  Print(env, Char('\n').Evict(env), Stream::StreamDesignator(env, stream),
        false);
}

} /* namespace core */
} /* namespace libmu */
