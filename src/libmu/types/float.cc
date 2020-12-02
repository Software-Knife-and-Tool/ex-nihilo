/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  float.cc: library floats
 **
 **/
#include "libmu/types/float.h"

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

#include "libmu/types/stream.h"

namespace libmu {

/** * view of float object **/
Type::TagPtr Float::ViewOf(Env* env, TagPtr flt) {
  assert(IsType(flt));
  
  auto view = std::vector<TagPtr>{
    Symbol::Keyword("float"),
    Fixnum(ToUint64(flt) >> 3).Evict(env, "float:view-address"),
    flt
  };
    
  return Vector(env, view).tag_;
}

/** * print float object to stream **/
void Float::Print(Env* env, TagPtr lfloat, TagPtr stream, bool) {
  assert(IsType(lfloat));
  assert(Stream::IsType(stream));

  std::ostringstream str;

  str << std::fixed << std::setprecision(6) << Float::FloatOf(lfloat);
  PrintStdString(env, str.str(), stream, false);
}

} /* namespace libmu */