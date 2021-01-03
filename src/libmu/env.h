/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  env.h: library environment
 **
 **/
#if !defined(LIBMU_ENV_H_)
#define LIBMU_ENV_H_

#include <cassert>
#include <memory>
#include <stack>

#include "libmu/platform/platform.h"

#include "libmu/heap/heap.h"
#include "libmu/type.h"
#include "libmu/types/namespace.h"

namespace libmu {
namespace core {

using platform::Platform;

class Namespace;

using Tag = Type::Tag;
using SYS_CLASS = Type::SYS_CLASS;

/** * Environment Class **/
class Env {
 public:
  /** * allocate from heap **/
  template <typename T>
  T* heap_alloc(size_t len, SYS_CLASS tag) {
    return reinterpret_cast<T*>(heap_->Alloc(len, tag));
  }

 public:
  /** * function activation record **/
  typedef struct frame {
    frame(Env* env, Tag frame_id, Tag func, Tag* argv, size_t nargs)
        : env(env),
          frame_id(frame_id),
          func(func),
          argv(argv),
          nargs(nargs),
          value(Type::NIL) {}

    ~frame() {}

    Env* env;     /* environment */
    Tag frame_id; /* frame id */
    Tag func;     /* applied function */
    Tag* argv;    /* argument list */
    size_t nargs; /* length of argument list */
    Tag value;    /* return value */

  } Frame;

 public:
  /** * mu core function implementation **/
  typedef std::function<void(Frame*)> FrameFn;

  /** * core function dispatch **/
  typedef struct {
    const char* name;
    FrameFn fn;
    size_t nreqs;
  } TagFn;

  /** * map address to core function **/
  TagFn* CoreFunction(Tag caddr) { return Type::Untag<TagFn>(caddr); }

 private:
  std::unordered_map<Tag, std::stack<Frame*>> framecache_;

 public:
  std::map<std::string, Tag> namespaces_;
  std::unique_ptr<heap::Heap> heap_; /* heap */
  Platform* platform_;               /* platform */
  std::vector<Frame*> frames_;       /* frame stack */
  size_t frame_id_;                  /* frame cache */
  std::vector<Tag> lexenv_;          /* lexical symbols */
                                     /* syntax dispatch */
  std::unordered_map<Tag, Tag> readtable_;
  Tag mu_;              /* mu namespace */
  Tag namespace_;       /* current namespace */
  Tag standard_input_;  /* standard input */
  Tag standard_output_; /* standard output */
  Tag standard_error_;  /* standard error */

 public: /* frame stack */
  constexpr void PushFrame(Frame* fp) { frames_.push_back(fp); }
  constexpr void PopFrame() { frames_.pop_back(); }
  constexpr void Cache(Frame* fp) { framecache_[fp->frame_id].push(fp); }
  constexpr void UnCache(Frame* fp) { framecache_[fp->frame_id].pop(); }

  /** * map id to frame **/
  constexpr Frame* MapFrame(Tag id) {
    assert(!framecache_[id].empty());
    return framecache_[id].top();
  }

  static Tag LastFrame(Env*);

 public: /* heap */
  static size_t Gc(Env*);
  static void GcFrame(Frame*);
  static void GcMark(Env*, Tag);

  static Tag MapNamespace(Env*, const std::string&);
  static void AddNamespace(Env*, Tag);

  static Tag EnvView(Env*);
  static Tag SystemTime(Env*);
  static Tag RunTime(Env*);

  static bool InHeap(Env* env, Tag ptr) {
    return Type::IsImmediate(ptr) ? false
                                  : env->heap_->in_heap(Type::ToAddress(ptr));
  }

  static Tag ViewOf(Env*, Tag);

 public: /* object model */
  explicit Env(Platform*, Platform::StreamId, Platform::StreamId,
               Platform::StreamId);
  ~Env() {}

}; /* class Env */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_ENV_H_ */
