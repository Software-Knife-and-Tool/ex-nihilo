/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  stream.h: library streams
 **
 **/
#if !defined(_LIBMU_TYPES_STREAM_H_)
#define _LIBMU_TYPES_STREAM_H_

#include <cassert>
#include <memory>
#include <string>
#include <typeinfo>

#include "platform/platform.h"

#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/heap/heap.h"

namespace libmu {

using platform::Platform;

class Env;

/** * stream type class **/
class Stream : public Type {
 public:
  typedef struct {
    Platform::StreamId stream;
    TagPtr fn;
  } Layout;

  Layout stream_;

 public: /* TagPtr */
  static constexpr bool IsType(TagPtr ptr) {
    return IsExtended(ptr) && Heap::SysClass(ptr) == SYS_CLASS::STREAM;
  }

  static bool IsFunction(TagPtr);

  static constexpr bool IsStreamDesignator(TagPtr ptr) {
    return Eq(ptr, T) || Eq(ptr, NIL) || IsType(ptr);
  }

  static Platform::StreamId streamId(TagPtr stream) {
    assert(IsType(stream));

    return Untag<Layout>(stream)->stream;
  }

  static TagPtr func(TagPtr stream) {
    assert(IsType(stream));

    return Untag<Layout>(stream)->fn;
  }

  static TagPtr MakeInputFile(Env* env, std::string path) {
    auto stream = Platform::OpenInputFile(path);

    if (stream == -1) return NIL;

    return Stream(stream).Evict(env, "stream:make-input-file");
  }

  static TagPtr MakeOutputFile(Env* env, std::string path) {
    return Stream(Platform::OpenOutputFile(path)).Evict(env, "stream:make-output-file");
  }

  static TagPtr MakeInputString(Env* env, std::string str) {
    return Stream(Platform::OpenInputString(str)).Evict(env, "stream:make-input-string");
  }

  static TagPtr MakeOutputString(Env* env, std::string init_string) {
    return Stream(Platform::OpenOutputString(init_string)).Evict(env, "stream:make-output-string");
  }

  static TagPtr StreamDesignator(Env*, TagPtr);
  static TagPtr Close(TagPtr);

  static TagPtr ReadByte(Env*, TagPtr);
  static TagPtr UnReadByte(TagPtr, TagPtr);
  static void WriteByte(TagPtr, TagPtr);

  static bool IsClosed(TagPtr);
  static bool IsEof(TagPtr);
  static void Flush(TagPtr);

  static void GcMark(Env*, TagPtr);
  static TagPtr ViewOf(Env*, TagPtr);

 public: /* object model */
  TagPtr Evict(Env*, const char*);

  explicit Stream(Platform::StreamId);
  explicit Stream(TagPtr);

}; /* class Stream */

} /* namespace libmu */

#endif /* _LIBMU_TYPES_STREAM_H_ */