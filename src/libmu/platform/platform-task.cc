/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 ** task.cc: process abstraction
 **
 **/
#include <cassert>
#include <cstdio>

#include "libmu/platform/platform.h"

namespace libmu {
namespace platform {

void Platform::TaskCreate() {}
void Platform::TaskKill() {}

} /* namespace platform */
} /* namespace libmu */
