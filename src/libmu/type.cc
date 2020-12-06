/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  type.cc: library tagged pointers
 **
 **/
#include "libmu/type.h"

#include <cassert>
#include <functional>
#include <iomanip>
#include <map>
#include <vector>

#include "libmu/print.h"
#include "libmu/read.h"

#include "libmu/types/address.h"
#include "libmu/types/cons.h"
#include "libmu/types/exception.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/float.h"
#include "libmu/types/function.h"
#include "libmu/types/macro.h"
#include "libmu/types/stream.h"
#include "libmu/types/struct.h"
#include "libmu/types/symbol.h"
#include "libmu/types/vector.h"

namespace libmu {
namespace {

static const std::map<TagPtr, SYS_CLASS> kSymbolMap{
    {Symbol::Keyword("address"), SYS_CLASS::ADDRESS},
    {Symbol::Keyword("byte"), SYS_CLASS::BYTE},
    {Symbol::Keyword("char"), SYS_CLASS::CHAR},
    {Symbol::Keyword("cons"), SYS_CLASS::CONS},
    {Symbol::Keyword("except"), SYS_CLASS::EXCEPTION},
    {Symbol::Keyword("fixnum"), SYS_CLASS::FIXNUM},
    {Symbol::Keyword("float"), SYS_CLASS::FLOAT},
    {Symbol::Keyword("func"), SYS_CLASS::FUNCTION},
    {Symbol::Keyword("macro"), SYS_CLASS::MACRO},
    {Symbol::Keyword("namespc"), SYS_CLASS::NAMESPACE},
    {Symbol::Keyword("stream"), SYS_CLASS::STREAM},
    {Symbol::Keyword("string"), SYS_CLASS::STRING},
    {Symbol::Keyword("struct"), SYS_CLASS::STRUCT},
    {Symbol::Keyword("symbol"), SYS_CLASS::SYMBOL},
    {Type::T, SYS_CLASS::T},
    {Symbol::Keyword("vector"), SYS_CLASS::VECTOR}};

}  // namespace

/** * is type a class symbol? **/
bool Type::IsClassSymbol(TagPtr type_sym) {
  assert(Symbol::IsKeyword(type_sym));

  return kSymbolMap.count(type_sym) != 0;
}

/** * type symbol map **/
SYS_CLASS Type::MapSymbolClass(TagPtr type_sym) {
  assert(Symbol::IsKeyword(type_sym));
  assert(kSymbolMap.count(type_sym));

  return kSymbolMap.at(type_sym);
}

/** * class symbol map */
TagPtr Type::MapClassSymbol(SYS_CLASS sys_class) {
  static const std::map<SYS_CLASS, TagPtr> kTypeMap{
      {SYS_CLASS::ADDRESS, Symbol::Keyword("address")},
      {SYS_CLASS::BYTE, Symbol::Keyword("byte")},
      {SYS_CLASS::CHAR, Symbol::Keyword("char")},
      {SYS_CLASS::CONS, Symbol::Keyword("cons")},
      {SYS_CLASS::EXCEPTION, Symbol::Keyword("except")},
      {SYS_CLASS::FIXNUM, Symbol::Keyword("fixnum")},
      {SYS_CLASS::FLOAT, Symbol::Keyword("float")},
      {SYS_CLASS::FUNCTION, Symbol::Keyword("func")},
      {SYS_CLASS::MACRO, Symbol::Keyword("macro")},
      {SYS_CLASS::NAMESPACE, Symbol::Keyword("namespc")},
      {SYS_CLASS::STREAM, Symbol::Keyword("stream")},
      {SYS_CLASS::STRING, Symbol::Keyword("string")},
      {SYS_CLASS::STRUCT, Symbol::Keyword("struct")},
      {SYS_CLASS::SYMBOL, Symbol::Keyword("symbol")},
      {SYS_CLASS::T, Type::T},
      {SYS_CLASS::VECTOR, Symbol::Keyword("vector")}};

  assert(kTypeMap.count(sys_class));

  return kTypeMap.at(sys_class);
}

/** * type of tagged pointer **/
SYS_CLASS Type::TypeOf(TagPtr ptr) {
  static const std::vector<std::pair<bool (*)(TagPtr), SYS_CLASS>> kPredMap{
      {Address::IsType, SYS_CLASS::ADDRESS},
      {Char::IsType, SYS_CLASS::CHAR},
      {Cons::IsType, SYS_CLASS::CONS},
      {Exception::IsType, SYS_CLASS::EXCEPTION},
      {Fixnum::IsType, SYS_CLASS::FIXNUM},
      {Float::IsType, SYS_CLASS::FLOAT},
      {Function::IsType, SYS_CLASS::FUNCTION},
      {Macro::IsType, SYS_CLASS::MACRO},
      {Namespace::IsType, SYS_CLASS::NAMESPACE},
      {Stream::IsType, SYS_CLASS::STREAM},
      {String::IsType, SYS_CLASS::STRING},
      {Struct::IsType, SYS_CLASS::STRUCT},
      {Symbol::IsType, SYS_CLASS::SYMBOL},
      {Vector::IsType, SYS_CLASS::VECTOR}};

  auto el = std::find_if(
      kPredMap.begin(), kPredMap.end(),
      [ptr](const std::pair<bool (*)(TagPtr), SYS_CLASS> predicate) {
        return predicate.first(ptr);
      });

  assert(el != kPredMap.end());

  return el->second;
}

} /* namespace libmu */
