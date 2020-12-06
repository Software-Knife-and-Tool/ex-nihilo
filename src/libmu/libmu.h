/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  libmu.h: library API
 **
 **/

#if !defined(_LIBMU_LIBMU_H_)
#define _LIBMU_LIBMU_H_

#include <cassert>
#include <cinttypes>
#include <functional>
#include <memory>

#include "platform/platform.h"

using platform::Platform;

namespace libmu {
namespace api {

extern "C" {
void* t();
void* nil();
const char* version();
void* eval(void*, void*);
void* read_stream(void*, void*);
void* read_string(void*, std::string);
void* read_cstr(void*, const char*);
void print(void*, void*, void*, bool);
const char* print_cstr(void*, void*, bool);
void terpri(void*, void*);
void withException(void*, std::function<void(void*)>);
void* env_default(Platform*);
void* env(Platform*, Platform::StreamId, Platform::StreamId,
          Platform::StreamId);
}

} /* namespace api */
} /* namespace libmu */

#endif /* _LIBMU_LIBMU_H_ */
