/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  core.h: core functions
 **
 **/
#if !defined(LIBMU_CORE_H_)
#define LIBMU_CORE_H_

#include <cassert>
#include <cinttypes>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "libmu/env.h"
#include "libmu/type.h"

namespace libmu {
namespace core {

using Tag = Type::Tag;

Tag Apply(Env*, Tag, Tag);
Tag Eval(Env*, Tag);

void Print(Env*, Tag, Tag, bool);
void PrintStdString(Env*, const std::string&, Tag, bool);
void Terpri(Env*, Tag);

Tag Read(Env*, Tag);
Tag ReadForm(Env*, Tag);
bool ReadWSUntilEof(Env*, Tag);

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_CORE_H_ */
