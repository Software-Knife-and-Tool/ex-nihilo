/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
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

#include "libmu/core.h"
#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/types/stream.h"

namespace libmu {
namespace core {

/** * view of float object **/
auto Float::ViewOf(Tag flt) -> Tag {
  assert(IsType(flt));

  auto view = std::vector<Tag>{Symbol::Keyword("float"),
                               Fixnum(ToUint64(flt) >> 3).tag_, flt};

  return Vector<Tag>(view).tag_;
}

/** * print float object to stream **/
auto Float::Print(Env* env, Tag lfloat, Tag stream, bool) -> void {
  assert(IsType(lfloat));
  assert(Stream::IsType(stream));

  std::ostringstream str;

  str << std::fixed << std::setprecision(6) << Float::FloatOf(lfloat);
  core::PrintStdString(env, str.str(), stream, false);
}

} /* namespace core */
} /* namespace libmu */
