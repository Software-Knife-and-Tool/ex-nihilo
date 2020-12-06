/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
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

#include "platform/platform.h"

#include "libmu/compiler.h"
#include "libmu/heap/heap.h"

#include "libmu/read.h"
#include "libmu/type.h"

#include "libmu/mu/mu.h"

#include "libmu/types/address.h"
#include "libmu/types/cons.h"
#include "libmu/types/exception.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/float.h"
#include "libmu/types/function.h"
#include "libmu/types/macro.h"
#include "libmu/types/namespace.h"
#include "libmu/types/stream.h"
#include "libmu/types/struct.h"
#include "libmu/types/symbol.h"
#include "libmu/types/vector.h"

namespace libmu {
namespace {

  static const std::vector<Env::TagPtrFn> kExtFuncTab{
    {"accept-socket-stream", mu::AcceptSocketStream, 1},
    {"acos", mu::Acos, 1},
    {"asin", mu::Asin, 1},
    {"atan", mu::Atan, 1},
    {"boundp", mu::IsBound, 1},
    {"car", mu::Car, 1},
    {"cdr", mu::Cdr, 1},
    {"char", mu::CharCoerce, 1},
    {"charp", mu::IsChar, 1},
    {"close", mu::Close, 1},
    {"closure", mu::Closure, 1},
    {"connect-socket-stream", mu::ConnectSocketStream, 1},
    {"cons", mu::MakeCons, 2},
    {"consp", mu::IsCons, 1},
    {"cos", mu::Cosine, 1},
    {"eofp", mu::IsEof, 1},
    {"eq", mu::Eq, 2},
    {"eval", mu::Eval, 1},
    {"exception", mu::MakeException, 3},
    {"exceptionp", mu::IsException, 1},
    {"exit", mu::Exit, 1},
    {"exp", mu::Exp, 1},
    {"find-ns", mu::FindNamespace, 1},
    {"find-symbol", mu::FindSymbolNamespace, 2},
    {"find-in-ns", mu::FindInNamespace, 3},
    {"fixnum", mu::FixnumCoerce, 1},
    {"fixnum*", mu::FixMul, 2},
    {"fixnum+", mu::FixAdd, 2},
    {"fixnum-", mu::FixSub, 2},
    {"fixnum/", mu::FixDiv, 2},
    {"fixnum<", mu::FixLessThan, 2},
    {"fixnump", mu::IsFixnum, 1},
    {"float", mu::FloatCoerce, 1},
    {"float*", mu::FloatMul, 2},
    {"float+", mu::FloatAdd, 2},
    {"float-", mu::FloatSub, 2},
    {"float/", mu::FloatDiv, 2},
    {"float<", mu::FloatLessThan, 2},
    {"floatp", mu::IsFloat, 1},
    {"functionp", mu::IsFunction, 1},
    {"gc", mu::Gc, 1},
    {"get-output-string-stream", mu::GetStringStream, 1},
    {"heap-info", mu::HeapInfo, 1},
    {"heap-log", mu::HeapLog, 1},
    {"in-ns", mu::SetNamespace, 1},
    {"intern", mu::InternNamespace, 4},
    {"invoke", mu::Invoke, 2},
    {"keyword", mu::MakeKeyword, 1},
    {"keywordp", mu::IsKeyword, 1},
    {"list-length", mu::ListLength, 1},
    {"load", mu::Load, 1},
    {"log", mu::Log, 1},
    {"log10", mu::Log10, 1},
    {"logand", mu::Logand, 2},
    {"logor", mu::Logor, 2},
    {"macro-function", mu::MacroFunction, 1},
    {"macroexpand", mu::MacroExpand, 1},
    {"make-symbol", mu::UninternedSymbol, 1},
    {"mod", mu::Mod, 2},
    {"namespacep", mu::IsNamespace, 1},
    {"ns", mu::MakeNamespace, 2},
    {"ns-current", mu::GetNamespace, 0},
    {"ns-import", mu::ImportOfNamespace, 1},
    {"ns-name", mu::NameOfNamespace, 1},
    {"ns-symbols", mu::NamespaceSymbols, 1},
    {"nth", mu::Nth, 2},
    {"nthcdr", mu::Nthcdr, 2},
    {"null", mu::IsNull, 1},
    {"open-function-stream", mu::FunctionStream, 1},
    {"open-input-file", mu::InFileStream, 1},
    {"open-input-string", mu::InStringStream, 1},
    {"open-output-file", mu::OutFileStream, 1},
    {"open-output-string", mu::OutStringStream, 1},
    {"open-socket-server", mu::SocketServerStream, 1},
    {"open-socket-stream", mu::SocketStream, 2},
    {"pow", mu::Pow, 2},
    {"raise", mu::Raise, 2},
    {"raise-exception", mu::RaiseException, 1},
    {"read", mu::Read, 1},
    {"read-byte", mu::ReadByte, 1},
    {"read-char", mu::ReadChar, 1},
    {"runtime", mu::RunTime, 0},
    {"set-macro-character", mu::SetMacroChar, 2},
    {"sin", mu::Sine, 1},
    {"special-operatorp", mu::IsSpecOp, 1},
    {"sqrt", mu::Sqrt, 1},
    {"stack-inspect", mu::StackInspect, 0},
    {"stack-trace", mu::StackTrace, 0},
    {"streamp", mu::IsStream, 1},
    {"struct", mu::MakeStruct, 2},
    {"struct-slots", mu::StructValues, 1},
    {"struct-type", mu::StructType, 1},
    {"structp", mu::IsStruct, 1},
    {"symbol-name", mu::SymbolName, 1},
    {"symbol-ns", mu::SymbolNamespace, 1},
    {"symbol-value", mu::SymbolValue, 1},
    {"symbolp", mu::IsSymbol, 1},
    {"system", mu::System, 1},
    {"system-env", mu::SystemEnv, 0},
    {"systime", mu::SystemTime, 0},
    {"tan", mu::Tangent, 1},
    {"terpri", mu::Terpri, 1},
    {"trampoline", mu::Trampoline, 1},
    {"type-of", mu::TypeOff, 1},
    {"unread-char", mu::UnReadChar, 2},
    {"vectorp", mu::IsVector, 1},
    {"view", mu::MakeView, 1},
    {"with-exception", mu::WithException, 2},
    {"write-byte", mu::WriteByte, 2},
    {"write-char", mu::WriteChar, 2}};

  static const std::vector<Env::TagPtrFn> kIntFuncTab{
    {"apply", mu::Apply, 2},
    {"block", mu::Block, 2},
    {"env-stack", mu::EnvStack, 2},
    {"env-stack-depth", mu::EnvStackDepth, 0},
    {"frame-ref", mu::FrameRef, 2},
    {"if", mu::TestIf, 3},
    {"letq", mu::Letq, 3},
    {"mapc", mu::MapC, 2},
    {"mapcar", mu::MapCar, 2},
    {"mapl", mu::MapL, 2},
    {"maplist", mu::MapList, 2},
    {"return", mu::Return, 2},
    {"print", mu::PrintEscape, 3},
    {"trampoline", mu::Trampoline, 1},
    {"vec-length", mu::VectorLength, 1},
    {"vec-map", mu::VectorMap, 2},
    {"vec-mapc", mu::VectorMapC, 2},
    {"vec-ref", mu::VectorRef, 2},
    {"vec-type", mu::VectorType, 1},
    {"vector", mu::MakeVector, 2}};

} /* anonymous namespace */

/** * make vector of frame **/
Type::TagPtr Env::FrameToVector(Env* env, Frame* fp) {

  auto args = std::vector<TagPtr>();
  
  for (size_t i = 0; i < fp->nargs; ++i)
    args.push_back(fp->argv[i]);
    
  auto frame = std::vector<TagPtr>{
    Symbol::Keyword("frame"),
    fp->func,
    Fixnum(static_cast<uint64_t>(fp->nargs)).tag_,
    Cons::List(env, args),    
    fp->frame_id
  };
  
  return Vector(env, frame).tag_;
}

/** * garbage collection **/
int Env::Gc(Env* env) {
  env->heap_->ClearRefBits();

  for (auto ns : env->namespaces_)
    GcMark(env, ns.second);

  for (auto fn : env->lexenv_)
    GcMark(env, fn);

  for (auto fp : env->frames_) {
    GcMark(env, fp->func);
    for (size_t i = 0; i < fp->nargs; ++i) GcMark(env, fp->argv[i]);
  }

  return env->heap_->Gc();
}

/** * garbage collection **/
void Env::GcMark(Env* env, TagPtr ptr) {
  std::function<void(Env*, TagPtr)> noGc = [](Env*, TagPtr) {};
  static const std::map<SYS_CLASS, std::function<void(Env*, TagPtr)>>
      kGcTypeMap{{SYS_CLASS::ADDRESS, noGc},
                 {SYS_CLASS::CHAR, noGc},
                 {SYS_CLASS::CONS, Cons::GcMark},
                 {SYS_CLASS::EXCEPTION, Exception::GcMark},
                 {SYS_CLASS::FIXNUM, noGc},
                 {SYS_CLASS::FLOAT, noGc},
                 {SYS_CLASS::FUNCTION, Function::GcMark},
                 {SYS_CLASS::MACRO, Macro::GcMark},
                 {SYS_CLASS::NAMESPACE, Namespace::GcMark},
                 {SYS_CLASS::NULLT, noGc},
                 {SYS_CLASS::STREAM, Stream::GcMark},
                 {SYS_CLASS::STRING, Vector::GcMark},
                 {SYS_CLASS::STRUCT, Struct::GcMark},
                 {SYS_CLASS::SYMBOL, Symbol::GcMark},
                 {SYS_CLASS::VECTOR, Vector::GcMark}};

  assert(kGcTypeMap.count(Type::TypeOf(ptr)));
  kGcTypeMap.at(Type::TypeOf(ptr))(env, ptr);
}

/** * make a view vector of pointer's contents **/
TagPtr Env::ViewOf(Env* env, TagPtr object) {
  static const std::map<SYS_CLASS, std::function<TagPtr(Env*, TagPtr)>>
      kViewMap{{SYS_CLASS::ADDRESS, Address::ViewOf},
               {SYS_CLASS::CHAR, Char::ViewOf},
               {SYS_CLASS::CONS, Cons::ViewOf},
               {SYS_CLASS::EXCEPTION, Exception::ViewOf},
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
TagPtr Env::MapNamespace(Env* env, std::string ns) {
  if (ns.empty()) {
    assert(Namespace::IsType(env->mu_));
    return env->namespace_;
  } else {
    auto ns_iter = env->namespaces_.find(ns);

    return (ns_iter == env->namespaces_.end()) ? Type::NIL : ns_iter->second;
  }
}

/** * add namespace object to environment map **/
void Env::AddNamespace(Env* env, TagPtr ns) {
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
  mu_ = Namespace(String(this, "mu").tag_, Type::NIL).Evict(this, "env:env");
  frame_id_ = 0;
  namespace_ = mu_;
  namespaces_["mu"] = mu_;

  standard_input_ = Symbol::Bind(
      Namespace::Intern(this, mu_, String(this, "standard-input").tag_),
      Stream(stdin).Evict(this, "env:stdin"));
  standard_output_ = Symbol::Bind(
      Namespace::Intern(this, mu_, String(this, "standard-output").tag_),
      Stream(stdout).Evict(this, "env:stdout"));
  standard_error_ = Symbol::Bind(
      Namespace::Intern(this, mu_, String(this, "error-output").tag_),
      Stream(stderr).Evict(this, "env:stderr"));

  env_.lexical = Type::NIL;

  for (auto& el : kExtFuncTab) {
    auto sym = Namespace::Intern(this, mu_, String(this, el.name).tag_);
    (void)Symbol::Bind(sym,
                       Function(this, sym, &el).Evict(this, "env:ext-fn"));
  }

  for (auto& el : kIntFuncTab) {
    auto sym = Namespace::InternInNs(this, mu_, String(this, el.name).tag_);
    (void)Symbol::Bind(sym,
                       Function(this, sym, &el).Evict(this, "env:int-fn"));
  }
}

} /* namespace libmu */
