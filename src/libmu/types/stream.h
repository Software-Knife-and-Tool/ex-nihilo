/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  stream.h: library streams
 **
 **/
#if !defined(LIBMU_TYPES_STREAM_H_)
#define LIBMU_TYPES_STREAM_H_

#include <cassert>
#include <memory>
#include <string>
#include <typeinfo>

#include "libmu/platform/platform.h"

#include "libmu/env.h"
#include "libmu/heapfmt.h"
#include "libmu/type.h"

#include "libmu/heap/heap.h"

namespace libmu {
namespace core {

using platform::Platform;

class Env;

/** * stream type class **/
class Stream : public Type {
 private:
  typedef struct {
    Platform::StreamId streamId;
    Tag fn;
  } Layout;

  Layout stream_;

 public: /* tag */
  static constexpr auto IsType(Tag ptr) -> bool {
    return IsExtended(ptr) &&
           HeapFmt<Layout>::SysClass(ptr) == Type::SYS_CLASS::STREAM;
  }

  static auto IsFunction(Tag) -> bool;

  static constexpr auto IsStreamDesignator(Tag ptr) -> bool {
    return Eq(ptr, T) || Eq(ptr, NIL) || IsType(ptr);
  }

  static auto streamId(Tag stream) -> Platform::StreamId {
    assert(IsType(stream));

    return Untag<Layout>(stream)->streamId;
  }

  static auto func(Tag stream) -> Tag {
    assert(IsType(stream));

    return Untag<Layout>(stream)->fn;
  }

  static auto MakeInputFile(Env* env, std::string path) -> Tag {
    auto stream = Platform::OpenInputFile(path);

    if (stream == -1) return NIL;

    return Stream(stream).Evict(env);
  }

  static auto MakeOutputFile(Env* env, std::string path) -> Tag {
    return Stream(Platform::OpenOutputFile(path)).Evict(env);
  }

  static auto MakeInputString(Env* env, std::string str) -> Tag {
    return Stream(Platform::OpenInputString(str)).Evict(env);
  }

  static auto MakeOutputString(Env* env, std::string init_string) -> Tag {
    return Stream(Platform::OpenOutputString(init_string)).Evict(env);
  }

  static auto StreamDesignator(Env*, Tag) -> Tag;
  static auto Close(Tag) -> Tag;

  static auto ReadByte(Env*, Tag) -> Tag;
  static auto UnReadByte(Tag, Tag) -> Tag;
  static auto WriteByte(Tag, Tag) -> void;

  static auto IsClosed(Tag) -> bool;
  static auto IsEof(Tag) -> bool;
  static auto Flush(Tag) -> void;

  static auto GcMark(Env*, Tag) -> void;
  static auto ViewOf(Env* env, Tag) -> Tag;

 public: /* type model */
  auto Evict(Env*) -> Tag;
  static auto EvictTag(Env*, Tag) -> Tag;

 public: /* object */
  explicit Stream(Platform::StreamId);
  explicit Stream(Tag);

}; /* class Stream */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TYPES_STREAM_H_ */
