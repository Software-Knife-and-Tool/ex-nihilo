/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
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

#include "libmu/platform/platform.h"

#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/heap/heap.h"

namespace libmu {
namespace core {

using platform::Platform;

class Env;

/** * stream type class **/
class Stream : public Type {
 public:
  typedef struct {
    Platform::StreamId stream;
    Tag fn;
  } Layout;

  Layout stream_;

 public: /* Tag */
  static constexpr bool IsType(Tag ptr) {
    return IsExtended(ptr) &&
           heap::Heap::SysClass(ptr) == Type::SYS_CLASS::STREAM;
  }

  static bool IsFunction(Tag);

  static constexpr bool IsStreamDesignator(Tag ptr) {
    return Eq(ptr, T) || Eq(ptr, NIL) || IsType(ptr);
  }

  static Platform::StreamId streamId(Tag stream) {
    assert(IsType(stream));

    return Untag<Layout>(stream)->stream;
  }

  static Tag func(Tag stream) {
    assert(IsType(stream));

    return Untag<Layout>(stream)->fn;
  }

  static Tag MakeInputFile(Env* env, std::string path) {
    auto stream = Platform::OpenInputFile(path);

    if (stream == -1) return NIL;

    return Stream(stream).Evict(env);
  }

  static Tag MakeOutputFile(Env* env, std::string path) {
    return Stream(Platform::OpenOutputFile(path)).Evict(env);
  }

  static Tag MakeInputString(Env* env, std::string str) {
    return Stream(Platform::OpenInputString(str)).Evict(env);
  }

  static Tag MakeOutputString(Env* env, std::string init_string) {
    return Stream(Platform::OpenOutputString(init_string)).Evict(env);
  }

  static Tag StreamDesignator(Env*, Tag);
  static Tag Close(Tag);

  static Tag ReadByte(Env*, Tag);
  static Tag UnReadByte(Tag, Tag);
  static void WriteByte(Tag, Tag);

  static bool IsClosed(Tag);
  static bool IsEof(Tag);
  static void Flush(Tag);

  static void GcMark(Env*, Tag);
  static Tag ViewOf(Env*, Tag);

 public: /* object model */
  Tag Evict(Env*);

  explicit Stream(Platform::StreamId);
  explicit Stream(Tag);

}; /* class Stream */

} /* namespace core */
} /* namespace libmu */

#endif /* _LIBMU_TYPES_STREAM_H_ */
