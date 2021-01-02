/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  readtable.cc: library readtable
 **
 **/
#include "libmu/readtable.h"

#include <cassert>
#include <map>
#include <vector>

#include "libmu/read.h"

namespace libmu {
namespace core {

/** * test for syntax char **/
auto SyntaxEq(Tag form, SYNTAX_CHAR syn) -> bool {
  return Char::IsType(form) ? static_cast<SYNTAX_CHAR>(form) == syn : false;
}

/** * map Tag to syntax type **/
auto MapSyntaxType(Tag ch) -> SYNTAX_TYPE {
  assert(Fixnum::IsType(ch));

  static const std::map<uint64_t, SYNTAX_TYPE> kTypeMap{
      {'\b', SYNTAX_TYPE::CONSTITUENT}, {'0', SYNTAX_TYPE::CONSTITUENT},
      {'1', SYNTAX_TYPE::CONSTITUENT},  {'2', SYNTAX_TYPE::CONSTITUENT},
      {'3', SYNTAX_TYPE::CONSTITUENT},  {'4', SYNTAX_TYPE::CONSTITUENT},
      {'5', SYNTAX_TYPE::CONSTITUENT},  {'6', SYNTAX_TYPE::CONSTITUENT},
      {'7', SYNTAX_TYPE::CONSTITUENT},  {'8', SYNTAX_TYPE::CONSTITUENT},
      {'9', SYNTAX_TYPE::CONSTITUENT},  {':', SYNTAX_TYPE::CONSTITUENT},
      {'<', SYNTAX_TYPE::CONSTITUENT},  {'>', SYNTAX_TYPE::CONSTITUENT},
      {'=', SYNTAX_TYPE::CONSTITUENT},  {'?', SYNTAX_TYPE::CONSTITUENT},
      {'!', SYNTAX_TYPE::CONSTITUENT},  {'@', SYNTAX_TYPE::CONSTITUENT},
      {'\t', SYNTAX_TYPE::WSPACE},      {'\n', SYNTAX_TYPE::WSPACE},
      {'\r', SYNTAX_TYPE::WSPACE},      {'\f', SYNTAX_TYPE::WSPACE},
      {' ', SYNTAX_TYPE::WSPACE},       {';', SYNTAX_TYPE::TMACRO},
      {'"', SYNTAX_TYPE::TMACRO},       {'#', SYNTAX_TYPE::MACRO},
      {'\'', SYNTAX_TYPE::TMACRO},      {'(', SYNTAX_TYPE::TMACRO},
      {')', SYNTAX_TYPE::TMACRO},       {'`', SYNTAX_TYPE::TMACRO},
      {',', SYNTAX_TYPE::TMACRO},       {'\\', SYNTAX_TYPE::ESCAPE},
      {'|', SYNTAX_TYPE::MESCAPE},      {'A', SYNTAX_TYPE::CONSTITUENT},
      {'B', SYNTAX_TYPE::CONSTITUENT},  {'C', SYNTAX_TYPE::CONSTITUENT},
      {'D', SYNTAX_TYPE::CONSTITUENT},  {'E', SYNTAX_TYPE::CONSTITUENT},
      {'F', SYNTAX_TYPE::CONSTITUENT},  {'G', SYNTAX_TYPE::CONSTITUENT},
      {'H', SYNTAX_TYPE::CONSTITUENT},  {'I', SYNTAX_TYPE::CONSTITUENT},
      {'J', SYNTAX_TYPE::CONSTITUENT},  {'K', SYNTAX_TYPE::CONSTITUENT},
      {'L', SYNTAX_TYPE::CONSTITUENT},  {'M', SYNTAX_TYPE::CONSTITUENT},
      {'N', SYNTAX_TYPE::CONSTITUENT},  {'O', SYNTAX_TYPE::CONSTITUENT},
      {'P', SYNTAX_TYPE::CONSTITUENT},  {'Q', SYNTAX_TYPE::CONSTITUENT},
      {'R', SYNTAX_TYPE::CONSTITUENT},  {'S', SYNTAX_TYPE::CONSTITUENT},
      {'T', SYNTAX_TYPE::CONSTITUENT},  {'V', SYNTAX_TYPE::CONSTITUENT},
      {'W', SYNTAX_TYPE::CONSTITUENT},  {'X', SYNTAX_TYPE::CONSTITUENT},
      {'Y', SYNTAX_TYPE::CONSTITUENT},  {'Z', SYNTAX_TYPE::CONSTITUENT},
      {'[', SYNTAX_TYPE::CONSTITUENT},  {']', SYNTAX_TYPE::CONSTITUENT},
      {'$', SYNTAX_TYPE::CONSTITUENT},  {'*', SYNTAX_TYPE::CONSTITUENT},
      {'{', SYNTAX_TYPE::CONSTITUENT},  {'}', SYNTAX_TYPE::CONSTITUENT},
      {'+', SYNTAX_TYPE::CONSTITUENT},  {'-', SYNTAX_TYPE::CONSTITUENT},
      {'/', SYNTAX_TYPE::CONSTITUENT},  {'~', SYNTAX_TYPE::CONSTITUENT},
      {'.', SYNTAX_TYPE::CONSTITUENT},  {'%', SYNTAX_TYPE::CONSTITUENT},
      {'&', SYNTAX_TYPE::CONSTITUENT},  {'^', SYNTAX_TYPE::CONSTITUENT},
      {'_', SYNTAX_TYPE::CONSTITUENT},  {'a', SYNTAX_TYPE::CONSTITUENT},
      {'b', SYNTAX_TYPE::CONSTITUENT},  {'c', SYNTAX_TYPE::CONSTITUENT},
      {'d', SYNTAX_TYPE::CONSTITUENT},  {'e', SYNTAX_TYPE::CONSTITUENT},
      {'f', SYNTAX_TYPE::CONSTITUENT},  {'g', SYNTAX_TYPE::CONSTITUENT},
      {'h', SYNTAX_TYPE::CONSTITUENT},  {'i', SYNTAX_TYPE::CONSTITUENT},
      {'j', SYNTAX_TYPE::CONSTITUENT},  {'k', SYNTAX_TYPE::CONSTITUENT},
      {'l', SYNTAX_TYPE::CONSTITUENT},  {'m', SYNTAX_TYPE::CONSTITUENT},
      {'n', SYNTAX_TYPE::CONSTITUENT},  {'o', SYNTAX_TYPE::CONSTITUENT},
      {'p', SYNTAX_TYPE::CONSTITUENT},  {'q', SYNTAX_TYPE::CONSTITUENT},
      {'r', SYNTAX_TYPE::CONSTITUENT},  {'s', SYNTAX_TYPE::CONSTITUENT},
      {'t', SYNTAX_TYPE::CONSTITUENT},  {'u', SYNTAX_TYPE::CONSTITUENT},
      {'v', SYNTAX_TYPE::CONSTITUENT},  {'w', SYNTAX_TYPE::CONSTITUENT},
      {'x', SYNTAX_TYPE::CONSTITUENT},  {'y', SYNTAX_TYPE::CONSTITUENT},
      {'z', SYNTAX_TYPE::CONSTITUENT}};

  assert(kTypeMap.count(Fixnum::Uint64Of(ch)) != 0);
  return kTypeMap.at(Fixnum::Uint64Of(ch));
}

/** * map fixnum to syntax char **/
auto MapSyntaxChar(Tag ch) -> SYNTAX_CHAR {
  assert(Fixnum::IsType(ch));

  static const std::map<uint64_t, SYNTAX_CHAR> kCharMap{
      {'\\', SYNTAX_CHAR::BACKSLASH}, {';', SYNTAX_CHAR::COMMENT},
      {'|', SYNTAX_CHAR::VBAR},       {'(', SYNTAX_CHAR::OPAREN},
      {')', SYNTAX_CHAR::CPAREN},     {'\'', SYNTAX_CHAR::QUOTE},
      {'"', SYNTAX_CHAR::STRING},     {'`', SYNTAX_CHAR::BACKQUOTE},
      {',', SYNTAX_CHAR::COMMA},      {'.', SYNTAX_CHAR::DOT},
      {'#', SYNTAX_CHAR::SHARP},      {':', SYNTAX_CHAR::COLON}};

  return kCharMap.count(Fixnum::Uint64Of(ch))
             ? kCharMap.at(Fixnum::Uint64Of(ch))
             : static_cast<SYNTAX_CHAR>(Fixnum::Uint64Of(ch));
}

} /* namespace core */
} /* namespace libmu */
