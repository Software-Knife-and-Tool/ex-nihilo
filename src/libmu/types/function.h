/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  function.h: library functions
 **
 **/
#if !defined(LIBMU_TYPES_FUNCTION_H_)
#define LIBMU_TYPES_FUNCTION_H_

#include <cassert>
#include <functional>
#include <memory>
#include <sstream>
#include <vector>

#include "libmu/compiler.h"
#include "libmu/core.h"
#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/types/address.h"
#include "libmu/types/cons.h"

namespace libmu {
namespace core {

class Env;
using Frame = Env::Frame;

/** * function type class **/
class Function : public Type {
 private:
  typedef struct {
    size_t arity; /* arity checking */
    Tag name;     /* debugging */
    Tag core;     /* as an address */
    Tag form;     /* as a lambda */
    Tag env;      /* closures */
    std::vector<Frame*> context;
    Tag frame_id; /* lexical reference */
  } HeapLayout;

  HeapLayout function_;

 public: /* Tag */
  static constexpr bool IsType(Tag ptr) { return TagOf(ptr) == TAG::FUNCTION; }

  /* accessors */
  static int arity(Tag fn) {
    assert(IsType(fn));

    return Untag<HeapLayout>(fn)->arity;
  }

  static std::vector<Frame*> context(Tag fn) {
    assert(IsType(fn));

    return Untag<HeapLayout>(fn)->context;
  }

  static size_t ncontext(Tag fn) {
    assert(IsType(fn));

    return Untag<HeapLayout>(fn)->context.size();
  }

  static std::vector<Frame*> context(Tag fn, std::vector<Frame*> ctx) {
    assert(IsType(fn));

    return Untag<HeapLayout>(fn)->context = ctx;
  }

  static Tag env(Tag fn) {
    assert(IsType(fn));

    return Untag<HeapLayout>(fn)->env;
  }

  static Tag env(Tag fn, Tag env) {
    assert(IsType(fn));

    return Untag<HeapLayout>(fn)->env = env;
  }

  static Tag form(Tag fn) {
    assert(IsType(fn));

    return Untag<HeapLayout>(fn)->form;
  }

  static Tag form(Tag fn, Tag form) {
    assert(IsType(fn));

    return Untag<HeapLayout>(fn)->form = form;
  }

  static Tag core(Tag fn) {
    assert(IsType(fn));

    return Untag<HeapLayout>(fn)->core;
  }

  static Tag frame_id(Tag fn) {
    assert(IsType(fn));

    return Untag<HeapLayout>(fn)->frame_id;
  }

  static Tag name(Tag fn) {
    assert(IsType(fn));

    return Untag<HeapLayout>(fn)->name;
  }

  static Tag name(Tag fn, Tag symbol) {
    assert(IsType(fn));
    assert(Symbol::IsType(symbol));

    return Untag<HeapLayout>(fn)->name = symbol;
  }

  /* arity coding */
  static constexpr size_t arity_nreqs(size_t arity) { return arity >> 1; }
  static constexpr bool arity_rest(size_t arity) { return arity & 1; }

  static size_t arity_nreqs(Tag fn) {
    assert(IsType(fn));

    return arity_nreqs(arity(fn));
  }

  static bool arity_rest(Tag fn) {
    assert(IsType(fn));

    return arity_rest(arity(fn));
  }

  static size_t arity_of(Env* env, Tag lambda) {
    assert(Cons::IsList(lambda));

    auto nsyms = Cons::Length(env, Cons::car(lambda));
    auto rest = !Null(Cons::cdr(lambda));

    return static_cast<size_t>(((nsyms - rest) << 1) | rest);
  }

  static Tag Funcall(Env*, Tag, const std::vector<Tag>&);
  static void GcMark(Env*, Tag);
  static Tag ViewOf(Env*, Tag);
  static void Print(Env*, Tag, Tag, bool);

 public: /* object model */
  Tag Evict(Env* env) {
    auto fp =
        env->heap_alloc<HeapLayout>(sizeof(HeapLayout), SYS_CLASS::FUNCTION);

    *fp = function_;
    tag_ = Entag(fp, TAG::FUNCTION);
    return tag_;
  }

  explicit Function(Env*, Tag, const Env::TagFn*);
  explicit Function(Env*, Tag, std::vector<Frame*>, Tag, Tag);

}; /* class Function */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TYPES_FUNCTION_H_ */
