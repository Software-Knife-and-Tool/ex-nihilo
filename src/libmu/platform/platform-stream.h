/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  stream.h: platform streams
 **
 **/

#if !defined(LIBMU_PLATFORM_STREAM_H_)
#define LIBMU_PLATFORM_STREAM_H_

#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "libmu/platform/platform.h"

namespace libmu {
namespace platform {

typedef struct {
  unsigned flags;
  union {
    Platform::STD_STREAM stdstream;
    std::istream *istream;
    std::ostream *ostream;
    std::stringstream *sstream;
    int sostream;
  } u;
} Stream;

static const int STREAM_INPUT = 0x0001;
static const int STREAM_OUTPUT = 0x0002;
static const int STREAM_IOS = 0x0004;
static const int STREAM_STRING = 0x0008;
static const int STREAM_FILE = 0x0010;
static const int STREAM_STD = 0x0020;
static const int STREAM_SOCKET = 0x0040;
static const int STREAM_CLOSED = 0x0080;

static Platform::StreamId StreamIdOf(Stream *ms) {
  return reinterpret_cast<Platform::StreamId>(ms);
}

static Stream *StructOfStreamId(Platform::StreamId sh) {
  return reinterpret_cast<Stream *>(sh);
}

} /* namespace platform */
} /* namespace libmu */

#endif /* LIBMU_PLATFORM_STREAM_H_ */
