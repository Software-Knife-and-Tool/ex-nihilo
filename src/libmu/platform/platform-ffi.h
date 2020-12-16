/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  ffi.h: foreign function interface
 **
 **/

#if !defined(_LIBMU_PLATFORM_FFI_H_)
#define _LIBMU_PLATFORM_FFI_H_

#include <cassert>
#include <cinttypes>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace libmu {
namespace platform {

class Ffi {
 public:
  typedef std::pair<std::string, int (*)(const std::vector<std::string> &)>
      FfiDesc;
};

} /* namespace platform */
} /* namespace libmu */

#endif /* _LIBMU_PLATFORM_FFI_H_ */
