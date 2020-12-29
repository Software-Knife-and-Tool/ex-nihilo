/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  read.h: libmu reader
 **
 **/
#if !defined(_LIBMU_READ_H_)
#define _LIBMU_READ_H_

#include <cassert>
#include <map>
#include <utility>
#include <vector>

#include "libmu/env.h"
#include "libmu/readtable.h"
#include "libmu/type.h"

#include "libmu/types/char.h"

namespace libmu {
namespace core {

TagPtr Read(Env*, TagPtr);
TagPtr ReadForm(Env*, TagPtr);
bool ReadWSUntilEof(Env*, TagPtr);

} /* namespace core */
} /* namespace libmu */

#endif /* _LIBMU_READ_H_ */
