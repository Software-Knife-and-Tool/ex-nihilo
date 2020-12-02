/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  function.h: library functions
 **
 **/
#if !defined(_LIBMU_TYPES_FUNCTION_H_)
#define _LIBMU_TYPES_FUNCTION_H_

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

class Env;
using Frame = Env::Frame;

/** * function type class **/
class Function : public Type {
 private:
  typedef struct {
    size_t nreqs;
    TagPtr core;
    TagPtr env;
    std::vector<Frame*>
           context;
    TagPtr lambda;
    TagPtr body;
    TagPtr frame_id;
    TagPtr name;
  } Layout;

  Layout function_;

 public: /* TagPtr */
  static constexpr bool IsType(TagPtr ptr) {
    return TagOf(ptr) == TAG::FUNCTION;
  }

  /* accessors */
  static size_t nreqs(TagPtr fn) {
    assert(IsType(fn));

    return
      Null(core(fn)) ? Untag<Layout>(fn)->nreqs
                     : Untag<Env::TagPtrFn>(core(fn))->nreqs;
  }
  
  static TagPtr lambda(TagPtr fn) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->lambda;
  }
  
  static std::vector<Frame*> context(TagPtr fn) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->context;
  }
  
  static size_t ncontext(TagPtr fn) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->context.size();
  }
  
  static std::vector<Frame*> context(TagPtr fn,
                                     std::vector<Frame*> ctx) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->context = ctx;
  }
  
  static TagPtr env(TagPtr fn) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->env;
  }
  
  static TagPtr env(TagPtr fn, TagPtr nenv) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->env = nenv;
  }

  static TagPtr core(TagPtr fn) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->core;
  }
  
  static TagPtr body(TagPtr fn) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->body;
  }
  
  static void body(TagPtr fn, TagPtr body) {
    assert(IsType(fn));

    Untag<Layout>(fn)->body = body;
  }
  
  static TagPtr frame_id(TagPtr fn) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->frame_id;
  }
  
  static TagPtr name(TagPtr fn) {
    assert(IsType(fn));

    return Untag<Layout>(fn)->name;
  }

  static TagPtr name(TagPtr fn, TagPtr symbol) {
    assert(IsType(fn));
    assert(Symbol::IsType(symbol));

    return Untag<Layout>(fn)->name = symbol;
  }

  static void CheckArity(Env*, TagPtr, const std::vector<TagPtr>&);
  static TagPtr Funcall(Env*, TagPtr, const std::vector<TagPtr>&);
  static void GcMark(Env*, TagPtr);
  static TagPtr ViewOf(Env*, TagPtr);

  static void CallFrame(Env::Frame* fp) {
    static Env::FrameFn exec = [](Env::Frame* fp) {
      fp->value = NIL;
      if (!Null(Function::body(fp->func)))
        Cons::MapC(fp->env,
                   [fp](Env* env, TagPtr form) {
                     fp->value = Eval(env, form);
                   },
                   Function::body(fp->func));
    };

    if (Null(core(fp->func)))
      exec(fp);
    else
      Untag<Env::TagPtrFn>(core(fp->func))->fn(fp);
  }

  static void Print(Env* env, TagPtr fn, TagPtr str, bool) {
    assert(IsType(fn));
    assert(Stream::IsType(str));

    auto stream = Stream::StreamDesignator(env, str);

    auto type = String::StdStringOf(
                  Symbol::name(Type::MapClassSymbol(Type::TypeOf(fn))));

    auto name = String::StdStringOf(Symbol::name(Function::name(fn)));
    
    std::stringstream hexs;

    hexs << std::hex << Type::to_underlying(fn);
    PrintStdString(env, "#<:" + type + " #x" + hexs.str() + " (" + name + ")>", stream, false);
  }

 public: /* object model */
  TagPtr Evict(Env* env, const char* src) {
    auto fp = env->heap_alloc<Layout>(sizeof(Layout), SYS_CLASS::FUNCTION, src);

    *fp = function_;
    tag_ = Entag(fp, TAG::FUNCTION);

    return tag_;
  }

  /* core functions */
  explicit Function(Env* env, const Env::TagPtrFn* core, TagPtr name) : Type() {
    assert(Symbol::IsType(name));
    
    function_.body = NIL;
    function_.core = Address(static_cast<void*>(const_cast<Env::TagPtrFn*>(core))).tag_;
    function_.context = std::vector<Frame*>{};
    function_.env = NIL;
    function_.frame_id = Fixnum(env->frame_id_).tag_;
    function_.lambda = NIL;
    function_.nreqs = 0;
    function_.name = name;
    
    env->frame_id_++;

    tag_ = Entag(reinterpret_cast<void*>(&function_), TAG::FUNCTION);
  }
  
  /* think: merge with closures */
  /* lambdas */
  explicit Function(Env* env, TagPtr lambda_list, TagPtr body, TagPtr name) : Type() {
    assert(Cons::IsList(lambda_list));
    assert(Symbol::IsType(name));
    
    auto lambda = Compiler::ParseLambda(env, lambda_list);

    size_t nreqs = Cons::Length(env, Compiler::lexicals(lambda)) -
                   (Null(Compiler::restsym(lambda)) ? 0 : 1);

#if 0
    static Env::FrameFn exec = [](Env::Frame* fp) {
      fp->value = NIL;
      if (!Null(Function::body(fp->func)))
        Cons::MapC(fp->env,
                   [fp](Env* env, TagPtr form) {
                     fp->value = Eval(env, form);
                   },
                   Function::body(fp->func));
    };
#endif
    
    function_.body = body;
    function_.core = NIL;
    function_.context = std::vector<Frame*>{};
    function_.env = Cons::List(env, env->lexenv_);
    function_.frame_id = Fixnum(env->frame_id_).tag_;
    function_.lambda = lambda;
    function_.nreqs = nreqs;
    function_.name = name;

    env->frame_id_++;

    tag_ = Entag(reinterpret_cast<void*>(&function_), TAG::FUNCTION);
  }

  /* closures */
  explicit Function(Env* env,
                    std::vector<Frame*> context,
                    TagPtr lambda_list,
                    TagPtr body) : Type() {
    assert(Cons::IsList(lambda_list));

    auto lambda = Compiler::ParseLambda(env, lambda_list);

    size_t nreqs = Cons::Length(env, Compiler::lexicals(lambda)) -
                   (Null(Compiler::restsym(lambda)) ? 0 : 1);

    static Env::FrameFn exec = [](Env::Frame* fp) {
      fp->value = NIL;
      if (!Null(Function::body(fp->func)))
        Cons::MapC(fp->env,
                   [fp](Env* env, TagPtr form) {
                     fp->value = Eval(env, form);
                   },
                   Function::body(fp->func));
    };

    function_.body = body;
    function_.core = NIL;
    function_.name = NIL;
    function_.context = context;
    function_.env = Cons::List(env, env->lexenv_);
    function_.frame_id = Fixnum(env->frame_id_).tag_;
    function_.lambda = lambda;
    function_.nreqs = nreqs;

    env->frame_id_++;

    tag_ = Entag(reinterpret_cast<void*>(&function_), TAG::FUNCTION);
  }

}; /* class Function */

} /* namespace libmu */

#endif /* _LIBMU_TYPES_FUNCTION_H_ */
