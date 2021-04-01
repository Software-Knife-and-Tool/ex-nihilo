/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
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

#include "libmu/core.h"
#include "libmu/type.h"

#include "libmu/types/stream.h"

namespace libmu {
namespace core {

/** * view of fixnum object **/
auto Fixnum::ViewOf(Tag fix) -> Tag {
  assert(IsType(fix));

  auto view = std::vector<Tag>{Symbol::Keyword("fixnum"), fix,
                               Fixnum(ToUint64(fix) >> 3).tag_};

  return VectorT<Tag>(view).tag_;
}

/** * print fixnum to stream **/
auto Fixnum::Print(Env* env, Tag fixnum, Tag stream, bool) -> void {
  assert(IsType(fixnum));
  assert(Stream::IsType(stream));

  std::ostringstream str;

  str << std::setbase(10) << Int64Of(fixnum);
  core::PrintStdString(env, str.str(), stream, false);
}

} /* namespace core */
} /* namespace libmu */
