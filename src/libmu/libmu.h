/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  libmu.h: library API
 **
 **/

#if !defined(LIBMULIBMU_H_)
#define LIBMULIBMU_H_

#include <cassert>
#include <cinttypes>
#include <functional>
#include <memory>

#include "libmu/platform/platform.h"

using libmu::platform::Platform;

namespace libmu {
namespace api {

extern "C" {
uintptr_t t();
uintptr_t nil();
const char* version();
uintptr_t eval(void*, uintptr_t);
uintptr_t read_stream(void*, uintptr_t);
uintptr_t read_string(void*, const std::string&);
uintptr_t read_cstr(void*, const char*);
void print(void*, uintptr_t, uintptr_t, bool);
const char* print_cstr(void*, uintptr_t, bool);
void terpri(void*, uintptr_t);
void withCondition(void*, const std::function<void(void*)>&);
uintptr_t env_default(Platform*);
uintptr_t env(Platform*, Platform::StreamId, Platform::StreamId,
              Platform::StreamId);
}

} /* namespace api */
} /* namespace libmu */

#endif /* LIBMULIBMU_H_ */
