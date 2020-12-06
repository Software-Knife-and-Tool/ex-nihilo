/*******
 **
 ** Copyright (c) 2017, James M. Putnam
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are met:
 **
 ** 1. Redistributions of source code must retain the above copyright notice,
 **    this list of conditions and the following disclaimer.
 **
 ** 2. Redistributions in binary form must reproduce the above copyright
 **    notice, this list of conditions and the following disclaimer in the
 **    documentation and/or other materials provided with the distribution.
 **
 ** 3. Neither the name of the copyright holder nor the names of its
 **    contributors may be used to endorse or promote products derived from
 **    this software without specific prior written permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 ** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 ** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 ** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 ** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 ** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 ** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 ** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 ** POSSIBILITY OF SUCH DAMAGE.
 **
 *******/

/********
 **
 **  stream.h: platform streams
 **
 **/

#if !defined(_PLATFORM_STREAM_H_)
#define _PLATFORM_STREAM_H_

#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "platform/platform.h"

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

#endif /* _PLATFORM_STREAM_H_ */
