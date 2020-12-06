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

extern "C" {
void* libmu_t();
void* libmu_nil();
const char* libmu_version();
void* libmu_eval(void*, void*);
void* libmu_read_stream(void*, void*);
void* libmu_read_string(void*, std::string);
void* libmu_read_cstr(void*, const char*);
void libmu_print(void*, void*, void*, bool);
const char* libmu_print_cstr(void*, void*, bool);
void libmu_terpri(void*, void*);
void libmu_withException(void*, std::function<void(void*)>);
void* libmu_env_default(Platform*);
void* libmu_env(Platform*, Platform::StreamId, Platform::StreamId,
                Platform::StreamId);
}

#endif /* _LIBMU_LIBMU_H_ */
