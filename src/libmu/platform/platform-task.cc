/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 ** task.cc: process abstraction
 **
 **/

#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cassert>
#include <cstdio>

#include "libmu/platform/platform.h"

namespace libmu {
namespace platform {

void Platform::TaskCreate() {}
void Platform::TaskKill() {}

} /* namespace platform */
} /* namespace libmu */
