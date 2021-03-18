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
    Tag mu;       /* as an address */
    Tag form;     /* as a lambda */
    Tag env;      /* closures */
    Tag frame_id; /* lexical reference */
    std::vector<Frame*> context;
  } HeapLayout;

  HeapLayout function_;

 public: /* Tag */
  static constexpr bool IsType(Tag ptr) { return TagOf(ptr) == TAG::FUNCTION; }

  /* accessors */
  static auto arity(Tag fn) -> size_t {
    assert(IsType(fn));

    return Untag<HeapLayout>(fn)->arity;
  }

  static auto context(Tag fn) -> std::vector<Frame*> {
    assert(IsType(fn));

    return Untag<HeapLayout>(fn)->context;
  }

  static auto ncontext(Tag fn) -> size_t {
    assert(IsType(fn));

    return Untag<HeapLayout>(fn)->context.size();
  }

  static auto context(Tag fn, std::vector<Frame*> ctx) -> std::vector<Frame*> {
    assert(IsType(fn));

    Untag<HeapLayout>(fn)->context = ctx;
    return ctx;
  }

  static auto env(Tag fn) -> Tag {
    assert(IsType(fn));

    return Untag<HeapLayout>(fn)->env;
  }

  static auto env(Tag fn, Tag env) -> Tag {
    assert(IsType(fn));

    Untag<HeapLayout>(fn)->env = env;
    return env;
  }

  static auto form(Tag fn) -> Tag {
    assert(IsType(fn));

    return Untag<HeapLayout>(fn)->form;
  }

  static auto form(Tag fn, Tag form) -> Tag {
    assert(IsType(fn));

    Untag<HeapLayout>(fn)->form = form;
    return form;
  }

  static auto mu(Tag fn) -> Tag {
    assert(IsType(fn));

    return Untag<HeapLayout>(fn)->mu;
  }

  static auto frame_id(Tag fn) -> Tag {
    assert(IsType(fn));

    return Untag<HeapLayout>(fn)->frame_id;
  }

  static auto name(Tag fn) -> Tag {
    assert(IsType(fn));

    return Untag<HeapLayout>(fn)->name;
  }

  static auto name(Tag fn, Tag symbol) -> Tag {
    assert(IsType(fn));
    assert(Symbol::IsType(symbol));

    Untag<HeapLayout>(fn)->name = symbol;
    return symbol;
  }

  static auto Funcall(Env*, Tag, const std::vector<Tag>&) -> Tag;
  static auto GcMark(Env*, Tag) -> void;
  static auto ViewOf(Env*, Tag) -> Tag;
  static auto Print(Env*, Tag, Tag, bool) -> void;

 public: /* type model */
  auto Evict(Env* env) -> Tag {
    auto fp =
        env->heap_alloc<HeapLayout>(sizeof(HeapLayout), SYS_CLASS::FUNCTION);

    *fp = function_;
    tag_ = Entag(fp, TAG::FUNCTION);
    return tag_;
  }

 public: /* object */
  explicit Function(Env* env, Tag name, const Env::TagFn* mu) : Type() {
    assert(Symbol::IsType(name));

    function_.arity = mu->nreqs << 1;
    function_.context = std::vector<Frame*>{};
    function_.mu =
        Address(static_cast<void*>(const_cast<Env::TagFn*>(mu))).tag_;
    function_.env = NIL;
    function_.form = NIL;
    function_.frame_id = Fixnum(env->frame_id_).tag_;
    function_.name = name;

    env->frame_id_++;

    tag_ = Entag(reinterpret_cast<void*>(&function_), TAG::FUNCTION);
  }

  explicit Function(Env* env, Tag name, std::vector<Frame*> context, Tag lambda,
                    Tag form)
      : Type() {
    assert(Cons::IsList(form));

    auto arity_of = [env, lambda]() -> size_t {
      auto nsyms = Cons::Length(env, Cons::car(lambda));
      auto rest = !Type::Null(Cons::cdr(lambda));

      return static_cast<size_t>(((nsyms - rest) << 1) | rest);
    };

    function_.arity = arity_of();
    function_.context = context;
    function_.mu = NIL;
    function_.env = Cons::List(env, env->lexenv_);
    function_.form = form;
    function_.frame_id = Fixnum(env->frame_id_).tag_;
    function_.name = name;

    env->frame_id_++;

    tag_ = Entag(reinterpret_cast<void*>(&function_), TAG::FUNCTION);
  }
}; /* class Function */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TYPES_FUNCTION_H_ */
