/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  env.h: library environment
 **
 **/
#if !defined(_LIBMU_ENV_H_)
#define _LIBMU_ENV_H_

#include <cassert>
#include <memory>
#include <stack>

#include "platform/platform.h"

#include "libmu/type.h"

#include "libmu/heap/heap.h"

#include "libmu/types/namespace.h"

namespace libmu {

using platform::Platform;

class Heap;
class Namespace;

using TagPtr = Type::TagPtr;
using SYS_CLASS = Type::SYS_CLASS;

/** * Environment Class **/
class Env {
 public:
  /** * allocate from heap **/
  template <typename T>
  T* heap_alloc(size_t len, SYS_CLASS tag, const char* src) {
    return reinterpret_cast<T*>(heap_->Alloc(len, tag, src));
  }

 public:
  /** * function activation record **/
  typedef struct frame {
    frame(Env* env, TagPtr frame_id, TagPtr func, TagPtr* argv, size_t nargs)
        : env(env),
          frame_id(frame_id),
          func(func),
          argv(argv),
          nargs(nargs),
          value(Type::NIL) {}

    ~frame() {}

    Env* env;        /* environment */
    TagPtr frame_id; /* frame id */
    TagPtr func;     /* applied function */
    TagPtr* argv;    /* argument list */
    size_t nargs;    /* length of argument list */
    TagPtr value;    /* return value */

  } Frame;

 public:
  /** * mu core function implementation **/
  typedef std::function<void(Frame*)> FrameFn;

  /** * core function dispatch **/
  typedef struct {
    const char* name;
    FrameFn fn;
    size_t nreqs;
  } TagPtrFn;

  /** * map address to core function **/
  TagPtrFn* CoreFunction(TagPtr caddr) { return Type::Untag<TagPtrFn>(caddr); }

 private:
  std::unordered_map<TagPtr, std::stack<Frame*>> framecache_;
  std::map<std::string, TagPtr> namespaces_;

 public:
  std::unique_ptr<Heap> heap_; /* heap */
  Platform* platform_;         /* platform */
  std::vector<Frame*> frames_; /* frame stack */
  size_t frame_id_;            /* frame cache */
  std::vector<TagPtr> lexenv_; /* lexical symbols */
                               /* syntax dispatch */
  std::unordered_map<TagPtr, TagPtr> readtable_;
  TagPtr mu_;              /* mu namespace */
  TagPtr namespace_;       /* current namespace */
  TagPtr standard_input_;  /* standard input */
  TagPtr standard_output_; /* standard output */
  TagPtr standard_error_;  /* standard error */

 public: /* frame stack */
  constexpr void PushFrame(Frame* fp) { frames_.push_back(fp); }
  constexpr void PopFrame() { frames_.pop_back(); }
  constexpr void Cache(Frame* fp) { framecache_[fp->frame_id].push(fp); }
  constexpr void UnCache(Frame* fp) { framecache_[fp->frame_id].pop(); }

  /** * map id to frame **/
  constexpr Frame* MapFrame(TagPtr id) {
    assert(!framecache_[id].empty());
    return framecache_[id].top();
  }

  static TagPtr LastFrame(Env*);

 public: /* heap */
  static int Gc(Env*);
  static void GcMark(Env*, TagPtr);

  static TagPtr MapNamespace(Env*, std::string);
  static void AddNamespace(Env*, TagPtr);

  static bool InHeap(Env* env, TagPtr ptr) {
    return Type::IsImmediate(ptr) ? false
                                  : env->heap_->in_heap(Type::ToAddress(ptr));
  }

  static TagPtr ViewOf(Env*, TagPtr);

 public: /* object model */
  explicit Env(Platform*, Platform::StreamId, Platform::StreamId,
               Platform::StreamId);
  ~Env() {}

}; /* class Env */

} /* namespace libmu */

#endif /* _LIBMU_ENV_H_ */
