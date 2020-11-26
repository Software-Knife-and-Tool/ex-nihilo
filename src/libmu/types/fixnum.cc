/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  fixnum.cc: library fixnums
 **
 **/
#include "libmu/types/fixnum.h"

#include <bitset>
#include <cassert>
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>
#include <utility>

#include "libmu/print.h"
#include "libmu/type.h"

#include "libmu/types/stream.h"

namespace libmu {

/** * view of fixnum object **/
Type::TagPtr Fixnum::ViewOf(Env* env, TagPtr fix) {
  assert(IsType(fix));
  
  auto view = std::vector<TagPtr>{
    Symbol::Keyword("fixnum"),
    fix,
    Fixnum(ToUint64(fix) >> 3).tag_
  };
    
  return Vector(env, view).tag_;
}

/** * print fixnum to stream **/
void Fixnum::PrintFixnum(Env* env, TagPtr fixnum, TagPtr stream, bool) {
  assert(IsType(fixnum));
  assert(Stream::IsType(stream));

  std::ostringstream str;

  str << std::setbase(10) << Int64Of(fixnum);
  PrintStdString(env, str.str(), stream, false);
}

} /* namespace libmu */
