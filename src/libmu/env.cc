/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  env.cc: library environment
 **
 **/
#include <vector>

#include "libmu/env.h"

#include <cassert>
#include <functional>

#include "libmu/platform/platform.h"

#include "libmu/compiler.h"
#include "libmu/core.h"
#include "libmu/heap/heap.h"
#include "libmu/macro.h"
#include "libmu/type.h"

#include "libmu/mu/mu.h"

#include "libmu/types/address.h"
#include "libmu/types/condition.h"
#include "libmu/types/cons.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/float.h"
#include "libmu/types/function.h"
#include "libmu/types/namespace.h"
#include "libmu/types/stream.h"
#include "libmu/types/struct.h"
#include "libmu/types/symbol.h"
#include "libmu/types/vector.h"

namespace libmu {
namespace core {
namespace {

/** * library extern core functions **/
static const std::vector<Env::TagFn> kExtFuncTab{
    {"accept-socket-stream", mu::AcceptSocketStream, 1},
    {"acos", mu::Acos, 1},
    {"apply", mu::Apply, 2},
    {"asin", mu::Asin, 1},
    {"atan", mu::Atan, 1},
    {"boundp", mu::IsBound, 1},
    {"car", mu::Car, 1},
    {"cdr", mu::Cdr, 1},
    {"charp", mu::IsChar, 1},
    {"close", mu::Close, 1},
    {"closure", mu::Closure, 1},
    {"condition", mu::MakeCondition, 3},
    {"conditionp", mu::IsCondition, 1},
    {"connect-socket-stream", mu::ConnectSocketStream, 1},
    {"cons", mu::MakeCons, 2},
    {"consp", mu::IsCons, 1},
    {"cos", mu::Cosine, 1},
    {"current-ns", mu::GetNamespace, 0},
    {"eofp", mu::IsEof, 1},
    {"eq", mu::Eq, 2},
    {"eval", mu::Eval, 1},
    {"exp", mu::Exp, 1},
    {"find-in-ns", mu::FindInNamespace, 3},
    {"find-ns", mu::FindNamespace, 1},
    {"find-symbol", mu::FindSymbolNamespace, 2},
    {"fixnum*", mu::FixMul, 2},
    {"fixnum+", mu::FixAdd, 2},
    {"fixnum-", mu::FixSub, 2},
    {"fixnum<", mu::FixLessThan, 2},
    {"fixnump", mu::IsFixnum, 1},
    {"float*", mu::FloatMul, 2},
    {"float+", mu::FloatAdd, 2},
    {"float-", mu::FloatSub, 2},
    {"float/", mu::FloatDiv, 2},
    {"float<", mu::FloatLessThan, 2},
    {"floatp", mu::IsFloat, 1},
    {"floor", mu::Floor, 2},
    {"functionp", mu::IsFunction, 1},
    {"gc", mu::Gc, 1},
    {"get-output-stream-string", mu::GetStringStream, 1},
    {"in-ns", mu::SetNamespace, 1},
    {"intern", mu::InternNamespace, 4},
    {"keyword", mu::MakeKeyword, 1},
    {"keywordp", mu::IsKeyword, 1},
    {"length", mu::ListLength, 1},
    {"list-to-vector", mu::VectorCons, 2},
    {"load", mu::Load, 1},
    {"log", mu::Log, 1},
    {"log10", mu::Log10, 1},
    {"logand", mu::Logand, 2},
    {"logor", mu::Logor, 2},
    {"macro-function", mu::MacroFunction, 1},
    {"macroexpand", mu::MacroExpand, 1},
    {"make-symbol", mu::UninternedSymbol, 1},
    {"mapc", mu::MapC, 2},
    {"mapcar", mu::MapCar, 2},
    {"mapl", mu::MapL, 2},
    {"maplist", mu::MapList, 2},
    {"namespacep", mu::IsNamespace, 1},
    {"ns", mu::MakeNamespace, 2},
    {"ns-import", mu::ImportOfNamespace, 1},
    {"ns-name", mu::NameOfNamespace, 1},
    {"ns-symbols", mu::NamespaceSymbols, 1},
    {"nth", mu::Nth, 2},
    {"nthcdr", mu::Nthcdr, 2},
    {"open-input-file", mu::InFileStream, 1},
    {"open-input-string", mu::InStringStream, 1},
    {"open-output-file", mu::OutFileStream, 1},
    {"open-output-string", mu::OutStringStream, 1},
    {"open-socket-server", mu::SocketServerStream, 1},
    {"open-socket-stream", mu::SocketStream, 2},
    {"open-stream", mu::FunctionStream, 1},
    {"pow", mu::Pow, 2},
    {"print", mu::PrintEscape, 3},
    {"raise", mu::Raise, 2},
    {"raise-condition", mu::RaiseCondition, 1},
    {"read", mu::Read, 1},
    {"read-byte", mu::ReadByte, 1},
    {"read-char", mu::ReadChar, 1},
    {"set-macro-character", mu::SetMacroChar, 2},
    {"sin", mu::Sine, 1},
    {"special-operatorp", mu::IsSpecOp, 1},
    {"sqrt", mu::Sqrt, 1},
    {"streamp", mu::IsStream, 1},
    {"struct", mu::MakeStruct, 2},
    {"struct-slots", mu::StructValues, 1},
    {"struct-type", mu::StructType, 1},
    {"structp", mu::IsStruct, 1},
    {"symbol-name", mu::SymbolName, 1},
    {"symbol-ns", mu::SymbolNamespace, 1},
    {"symbol-value", mu::SymbolValue, 1},
    {"symbolp", mu::IsSymbol, 1},
    {"tan", mu::Tangent, 1},
    {"terpri", mu::Terpri, 1},
    {"trampoline", mu::Trampoline, 1},
    {"truncate", mu::Truncate, 2},
    {"type-of", mu::TypeOf, 1},
    {"unread-char", mu::UnReadChar, 2},
    {"vector-length", mu::VectorLength, 1},
    {"vector-map", mu::VectorMap, 2},
    {"vector-mapc", mu::VectorMapC, 2},
    {"vector-ref", mu::VectorRef, 2},
    {"vector-type", mu::VectorType, 1},
    {"vectorp", mu::IsVector, 1},
    {"view", mu::MakeView, 1},
    {"with-condition", mu::WithCondition, 2},
    {"write-byte", mu::WriteByte, 2},
    {"write-char", mu::WriteChar, 2}};

/** * library intern core functions **/
static const std::vector<Env::TagFn> kIntFuncTab{
    {"block", mu::Block, 2},        {"clocks", mu::Clocks, 0},
    {"env-view", mu::EnvView, 0},   {"exit", mu::Exit, 1},
    {"frame-ref", mu::FrameRef, 2}, {"heap-view", mu::HeapInfo, 1},
    {"invoke", mu::Invoke, 2},      {"letq", mu::Letq, 3},
    {"return", mu::Return, 2},      {"system", mu::System, 1}};

/** * make vector of frame **/
auto FrameView(Env* env, Frame* fp) {
  auto args = std::vector<Tag>();

  for (size_t i = 0; i < fp->nargs; ++i) args.push_back(fp->argv[i]);

  auto frame = std::vector<Tag>{Symbol::Keyword("frame"), fp->func,
                                Fixnum(static_cast<uint64_t>(fp->nargs)).tag_,
                                Cons::List(env, args), fp->frame_id};

  return Vector(env, frame).tag_;
}

/** * make vector of env stack **/
auto EnvStack(Env* env) {
  std::vector<Tag> stack;

  for (auto fp : env->frames_) {
    std::vector<Tag> frame;
    std::vector<Tag> args;

    frame.push_back(Symbol::Keyword("frame"));
    frame.push_back(fp->func);
    frame.push_back(Fixnum(fp->nargs).tag_);

    for (size_t i = 0; i < fp->nargs; ++i) args.push_back(fp->argv[i]);

    frame.push_back(Cons::List(fp->env, args));
    frame.push_back(fp->frame_id);

    stack.push_back(Vector(fp->env, frame).tag_);
  }

  return Cons::List(env, stack);
}

/** * make list of namespaces **/
auto Namespaces(Env* env) {
  std::vector<Tag> nslist;

  for (auto& ns : env->namespaces_) nslist.push_back(ns.second);

  return Cons::List(env, nslist);
}

} /* anonymous namespace */

/** * gc frame **/
void Env::GcFrame(Frame* fp) {
  GcMark(fp->env, fp->func);
  for (size_t i = 0; i < fp->nargs; ++i) Env::GcMark(fp->env, fp->argv[i]);
}

/** * gc environment **/
auto Env::Gc(Env* env) -> size_t {
  env->heap_->ClearRefBits();

  for (auto& ns : env->namespaces_) GcMark(env, ns.second);
  for (auto& fn : env->lexenv_) GcMark(env, fn);
  for (auto& fp : env->frames_) GcFrame(fp);

  return env->heap_->Gc();
}

/** grab last frame **/
auto Env::LastFrame(Env* env) -> Tag {
  return env->frames_.empty() ? Type::NIL
                              : FrameView(env, env->frames_.front());
}

/** * env view **/
auto Env::EnvView(Env* env) -> Tag {
  uint64_t st, rt;

  Platform::SystemTime(&st);
  Platform::ProcessTime(&rt);

  auto view = std::vector<Tag>{
      Symbol::Keyword("env"), env->namespace_, Namespaces(env), Fixnum(st).tag_,
      Fixnum(rt).tag_,        EnvStack(env),   env->src_};

  return Vector(env, view).tag_;
}

/** * garbage collection **/
auto Env::GcMark(Env* env, Tag ptr) -> void {
  std::function<void(Env*, Tag)> noGc = [](Env*, Tag) {};
  static const std::map<SYS_CLASS, std::function<void(Env*, Tag)>> kGcTypeMap{
      {SYS_CLASS::ADDRESS, noGc},
      {SYS_CLASS::CHAR, noGc},
      {SYS_CLASS::CONS, Cons::GcMark},
      {SYS_CLASS::CONDITION, Condition::GcMark},
      {SYS_CLASS::FIXNUM, noGc},
      {SYS_CLASS::FLOAT, noGc},
      {SYS_CLASS::FUNCTION, Function::GcMark},
      {SYS_CLASS::MACRO, Macro::GcMark},
      {SYS_CLASS::NAMESPACE, Namespace::GcMark},
      {SYS_CLASS::STREAM, Stream::GcMark},
      {SYS_CLASS::STRING, Vector::GcMark},
      {SYS_CLASS::STRUCT, Struct::GcMark},
      {SYS_CLASS::SYMBOL, Symbol::GcMark},
      {SYS_CLASS::VECTOR, Vector::GcMark}};

  assert(IsEvicted(env, ptr));
  assert(kGcTypeMap.count(Type::TypeOf(ptr)));
  kGcTypeMap.at(Type::TypeOf(ptr))(env, ptr);
}

/** * make a view vector of pointer's contents **/
auto Env::ViewOf(Env* env, Tag object) -> Tag {
  static const std::map<SYS_CLASS, std::function<Tag(Env*, Tag)>> kViewMap{
      {SYS_CLASS::ADDRESS, Address::ViewOf},
      {SYS_CLASS::CHAR, Char::ViewOf},
      {SYS_CLASS::CONS, Cons::ViewOf},
      {SYS_CLASS::CONDITION, Condition::ViewOf},
      {SYS_CLASS::FIXNUM, Fixnum::ViewOf},
      {SYS_CLASS::FUNCTION, Function::ViewOf},
      {SYS_CLASS::FLOAT, Float::ViewOf},
      {SYS_CLASS::MACRO, Macro::ViewOf},
      {SYS_CLASS::NAMESPACE, Namespace::ViewOf},
      {SYS_CLASS::STRING, String::ViewOf},
      {SYS_CLASS::STREAM, Stream::ViewOf},
      {SYS_CLASS::STRUCT, Struct::ViewOf},
      {SYS_CLASS::SYMBOL, Symbol::ViewOf},
      {SYS_CLASS::VECTOR, Vector::ViewOf}};

  auto view = kViewMap.count(Type::TypeOf(object)) != 0;
  return view ? kViewMap.at(Type::TypeOf(object))(env, object) : Type::NIL;
}

/** look up namespace by name in environment map **/
auto Env::MapNamespace(Env* env, const std::string& ns) -> Tag {
  if (ns.empty()) {
    assert(Namespace::IsType(env->mu_));
    return env->namespace_;
  } else {
    auto ns_iter = env->namespaces_.find(ns);

    return (ns_iter == env->namespaces_.end()) ? Type::NIL : ns_iter->second;
  }
}

/** * add namespace object to environment map **/
auto Env::AddNamespace(Env* env, Tag ns) -> void {
  assert(Namespace::IsType(ns));

  auto ns_name = String::StdStringOf(Namespace::name(ns));
  auto ns_iter = env->namespaces_.find(ns_name);

  if (ns_iter == env->namespaces_.end()) env->namespaces_[ns_name] = ns;
}

/** * env constructor **/
Env::Env(Platform* platform, Platform::StreamId stdin,
         Platform::StreamId stdout, Platform::StreamId stderr) {
  assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__);

  platform_ = platform;
  heap_ = std::make_unique<Heap>();
  mu_ = Namespace(String(this, "mu").tag_, Type::NIL).Evict(this);
  frame_id_ = 0;
  namespace_ = mu_;
  namespaces_["mu"] = mu_;
  nil_ = Type::NIL;
  src_ = Type::NIL;

  standard_input_ =
      Namespace::Intern(this, mu_, String(this, "standard-input").tag_,
                        Stream(stdin).Evict(this));

  standard_output_ =
      Namespace::Intern(this, mu_, String(this, "standard-output").tag_,
                        Stream(stdout).Evict(this));

  standard_error_ = Namespace::Intern(
      this, mu_, String(this, "error-output").tag_, Stream(stderr).Evict(this));

  for (auto& el : kExtFuncTab) {
    auto sym = Namespace::Intern(this, mu_, String(this, el.name).tag_);
    (void)Symbol::Bind(sym, Function(this, sym, &el).Evict(this));
  }

  for (auto& el : kIntFuncTab) {
    auto sym = Namespace::InternInNs(this, mu_, String(this, el.name).tag_);
    (void)Symbol::Bind(sym, Function(this, sym, &el).Evict(this));
  }
}

} /* namespace core */
} /* namespace libmu */
