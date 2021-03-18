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
void* t();
void* nil();
const char* version();
void* eval(void*, void*);
void* read_stream(void*, void*);
void* read_string(void*, const std::string&);
void* read_cstr(void*, const char*);
void print(void*, void*, void*, bool);
const char* print_cstr(void*, void*, bool);
void terpri(void*, void*);
void withCondition(void*, const std::function<void(void*)>&);
void* env_default(Platform*);
void* env(Platform*, Platform::StreamId, Platform::StreamId,
          Platform::StreamId);
}

} /* namespace api */
} /* namespace libmu */

#endif /* LIBMULIBMU_H_ */
