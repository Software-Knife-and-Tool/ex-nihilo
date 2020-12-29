/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 ** ffi.cc: platform foreign functions
 **
 **/

#include <assert.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <string>

#include "libmu/platform/platform-ffi.h"
#include "libmu/platform/platform.h"

namespace libmu {
namespace platform {} /* namespace platform */
} /* namespace libmu */
