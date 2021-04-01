
/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 ** stream.cc: library streams
 **
 **/
#include "libmu/types/stream.h"

#include <cassert>

#include <fstream>
#include <iostream>

#include "libmu/platform/platform.h"

#include "libmu/core.h"
#include "libmu/env.h"
#include "libmu/readtable.h"
#include "libmu/type.h"

#include "libmu/types/char.h"
#include "libmu/types/cons.h"

#include "libmu/types/condition.h"
#include "libmu/types/function.h"
#include "libmu/types/namespace.h"

namespace libmu {
namespace core {

/** * function stream? **/
auto Stream::IsFunction(Tag ptr) -> bool {
  return IsType(ptr) && Function::IsType(func(ptr));
}

/** * garbage collection **/
auto Stream::GcMark(Env* env, Tag stream) -> void {
  assert(IsType(stream));

  if (IsFunction(stream)) env->heap_->GcMark(func(stream));

  env->heap_->GcMark(stream);
}

/** * view of struct object **/
auto Stream::ViewOf(Tag stream) -> Tag {
  assert(IsType(stream));

  auto view = std::vector<Tag>{
      Symbol::Keyword("stream"), stream, Fixnum(ToUint64(stream) >> 3).tag_,
      Fixnum(streamId(stream)).tag_, Fixnum(ToUint64(func(stream))).tag_};

  return VectorT<Tag>(view).tag_;
}

/** * stream eof predicate **/
auto Stream::IsEof(Tag stream) -> bool {
  assert(Stream::IsType(stream));

  if (IsFunction(stream)) return false;

  return Platform::IsEof(streamId(stream));
}

/** * stream closed predicate **/
auto Stream::IsClosed(Tag stream) -> bool {
  assert(Stream::IsType(stream));

  if (IsFunction(stream)) return false;

  return Platform::IsClosed(streamId(stream));
}

/** * flush stream **/
auto Stream::Flush(Tag stream) -> void {
  assert(IsType(stream));

  if (IsFunction(stream)) return;

  Platform::Flush(streamId(stream));
}

/** * map stream designator onto a kernel stream **/
auto Stream::StreamDesignator(Env* env, Tag stream) -> Tag {
  switch (stream) {
    case T:
      stream = Symbol::value(env->standard_input_);
      break;
    case NIL:
      stream = Symbol::value(env->standard_output_);
      break;
    default:
      if (!Stream::IsType(stream))
        Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR,
                         "not a stream (stream-designator)", stream);
      break;
  }

  assert(Stream::IsType(stream));

  return stream;
}

/** * unread char from stream **/
auto Stream::UnReadByte(Tag byte, Tag stream) -> Tag {
  assert(Stream::IsType(stream));
  assert(!Stream::IsFunction(stream));
  assert(Fixnum::IsType(byte));

  Platform::UnReadByte(Fixnum::Int64Of(byte), streamId(stream));

  return byte;
}

/** * write byte to stream **/
auto Stream::WriteByte(Tag byte, Tag stream) -> void {
  assert(Fixnum::IsType(byte));
  assert(!Stream::IsFunction(stream));
  assert(Stream::IsType(stream));

  assert(Platform::IsOutput(streamId(stream)));
  assert(!Platform::IsClosed(streamId(stream)));

  Platform::WriteByte(static_cast<int>(Fixnum::Uint64Of(byte)),
                      streamId(stream));
}

/** * read byte from stream, returns a fixnum or nil **/
auto Stream::ReadByte(Env* env, Tag strm) -> Tag {
  auto stream = StreamDesignator(env, strm);
  assert(!Stream::IsFunction(stream));

  auto byte = Platform::ReadByte(streamId(stream));

  if (byte == -1 || byte == 0x4) return NIL;

  return Fixnum(byte).tag_;
}

/** * close stream **/
auto Stream::Close(Tag stream) -> Tag {
  assert(IsType(stream));
  assert(!Stream::IsFunction(stream));

  Platform::Close(streamId(stream));
  return T;
}

/** evict stream to heap **/
auto Stream::Evict(Env* env) -> Tag {
  auto hp = env->heap_alloc<HeapLayout>(sizeof(HeapLayout), SYS_CLASS::STREAM);

  *hp = stream_;
  hp->fn = Env::Evict(env, hp->fn);

  tag_ = Entag(hp, TAG::EXTEND);

  return tag_;
}

auto Stream::EvictTag(Env* env, Tag stream) -> Tag {
  assert(IsType(stream));
  assert(!Env::IsEvicted(env, stream));

  // printf("EvictTag: stream\n");
  auto hp = env->heap_alloc<HeapLayout>(sizeof(HeapLayout), SYS_CLASS::STREAM);
  auto sp = Untag<HeapLayout>(stream);

  *hp = *sp;
  hp->fn = Env::Evict(env, hp->fn);

  return Entag(hp, TAG::EXTEND);
}

Stream::Stream(Platform::StreamId streamid) : Type() {
  stream_.streamId = streamid;
  stream_.fn = NIL;

  tag_ = Entag(reinterpret_cast<void*>(&stream_), TAG::EXTEND);
}

Stream::Stream(Tag fn) : Type() {
  assert(Function::IsType(fn));

  stream_.streamId = Platform::STREAM_ERROR;
  stream_.fn = fn;

  tag_ = Entag(reinterpret_cast<void*>(&stream_), TAG::EXTEND);
}

} /* namespace core */
} /* namespace libmu */
