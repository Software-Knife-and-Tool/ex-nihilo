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
#include "libmu/env.h"
#include "libmu/eval.h"
#include "libmu/print.h"
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
  } Layout;

  Layout function_;

 public: /* Tag */
  static constexpr bool IsType(Tag ptr) { return TagOf(ptr) == TAG::FUNCTION; }

  /* accessors */
  static int arity(Tag fn) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->arity;
  }

  static std::vector<Frame*> context(Tag fn) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->context;
  }

  static size_t ncontext(Tag fn) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->context.size();
  }

  static std::vector<Frame*> context(Tag fn, std::vector<Frame*> ctx) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->context = ctx;
  }

  static Tag env(Tag fn) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->env;
  }

  static Tag env(Tag fn, Tag env) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->env = env;
  }

  static Tag form(Tag fn) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->form;
  }

  static Tag form(Tag fn, Tag form) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->form = form;
  }

  static Tag core(Tag fn) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->core;
  }

  static Tag frame_id(Tag fn) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->frame_id;
  }

  static Tag name(Tag fn) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->name;
  }

  static Tag name(Tag fn, Tag symbol) {
    assert(IsType(fn));
    assert(Symbol::IsType(symbol));

    return Untag<Layout>(fn)->name = symbol;
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

  static void CheckArity(Env*, Tag, const std::vector<Tag>&);

  static Tag Funcall(Env*, Tag, const std::vector<Tag>&);
  static void GcMark(Env*, Tag);
  static Tag ViewOf(Env*, Tag);

  static void CallFrame(Env::Frame* fp) {
    fp->value = NIL;
    if (Null(core(fp->func))) {
      Cons::cons_iter<Tag> iter(Cons::cdr(Function::form(fp->func)));
      for (auto it = iter.begin(); it != iter.end(); it = ++iter)
        fp->value = core::Eval(fp->env, it->car);
    } else
      Untag<Env::TagFn>(core(fp->func))->fn(fp);
  }

  static void Print(Env* env, Tag fn, Tag str, bool) {
    assert(IsType(fn));
    assert(Stream::IsType(str));

    auto stream = Stream::StreamDesignator(env, str);

    auto type = String::StdStringOf(
        Symbol::name(Type::MapClassSymbol(Type::TypeOf(fn))));

    auto name = String::StdStringOf(Symbol::name(Function::name(fn)));

    std::stringstream hexs;

    hexs << std::hex << Type::to_underlying(fn);
    core::PrintStdString(env,
                         "#<:" + type + " #x" + hexs.str() + " (" + name + ")>",
                         stream, false);
  }

 public: /* object model */
  Tag Evict(Env* env) {
    auto fp = env->heap_alloc<Layout>(sizeof(Layout), SYS_CLASS::FUNCTION);

    *fp = function_;
    tag_ = Entag(fp, TAG::FUNCTION);
    return tag_;
  }

  /* core functions */
  explicit Function(Env* env, Tag name, const Env::TagFn* core) : Type() {
    assert(Symbol::IsType(name));

    function_.arity = core->nreqs << 1;
    function_.context = std::vector<Frame*>{};
    function_.core =
        Address(static_cast<void*>(const_cast<Env::TagFn*>(core))).tag_;
    function_.env = NIL;
    function_.form = NIL;
    function_.frame_id = Fixnum(env->frame_id_).tag_;
    function_.name = name;

    env->frame_id_++;

    tag_ = Entag(reinterpret_cast<void*>(&function_), TAG::FUNCTION);
  }

  /* closures */
  explicit Function(Env* env, Tag name, std::vector<Frame*> context, Tag lambda,
                    Tag form)
      : Type() {
    assert(Cons::IsList(form));

    function_.arity = arity_of(env, lambda);
    function_.context = context;
    function_.core = NIL;
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
