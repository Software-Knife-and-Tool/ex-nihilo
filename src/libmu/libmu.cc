/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  libmu.cc: library API
 **
 **/
#include "libmu/libmu.h"

#include <cassert>
#include <cstdio>
#include <functional>
#include <memory>

#include "platform/platform.h"

#include "libmu/compiler.h"
#include "libmu/env.h"
#include "libmu/eval.h"
#include "libmu/print.h"
#include "libmu/read.h"
#include "libmu/type.h"

#include "libmu/heap/heap.h"

#include "libmu/types/exception.h"
#include "libmu/types/namespace.h"
#include "libmu/types/stream.h"
#include "libmu/types/string.h"
#include "libmu/types/symbol.h"

namespace libmu {
namespace api {

using Type = core::Type;
using Env = core::Env;

/** * constants interface **/
void* t() { return (void*)Type::T; }
void* nil() { return (void*)Type::NIL; }
const char* version() { return "0.0.21"; }

/** * read_stdin interface **/
void* read_stream(void* env, void* stream) {
  return (void*)core::Type::ToAddress(
      core::Read((Env*)env, core::Type::Entag(stream, Type::TAG::ADDRESS)));
}

/** * read interface **/
void* read_cstr(void* env, const char* src) {
  return (void*)Type::ToAddress(
      core::Read((Env*)env, core::Stream::MakeInputString((Env*)env, src)));
}

/** * read interface **/
void* read_string(void* env, std::string src) {
  return read_cstr(env, src.c_str());
}

/** * princ/prin1 interface **/
void print(void* env, void* ptr, void* stream, bool esc) {
  core::Print((Env*)env, Type::Entag(ptr, Type::TAG::ADDRESS),
              Type::Entag(stream, Type::TAG::ADDRESS), esc);
}

/** * print to a cstring **/
const char* print_cstr(void* env, void* ptr, bool esc) {
  auto stream = core::Stream::MakeOutputString((Env*)env, "");

  core::Print((Env*)env, Type::Entag(ptr, Type::TAG::ADDRESS), stream, esc);

  auto sp = Type::Untag<core::Stream::Layout>(stream);

  auto cp = Platform::GetStdString(sp->stream);
  auto cstr = new char[strlen(cp.c_str()) + 1];

  return strcpy(cstr, cp.c_str());
}

/** * print end of line **/
void terpri(void* env, void* stream) {
  core::Terpri((Env*)env, Type::Entag(stream, Type::TAG::ADDRESS));
}

/** * catch library throws **/
void withException(void* env, std::function<void(void*)> fn) {
  try {
    fn(env);
  } catch (Type::TagPtr exception) {
    auto std_error = core::Symbol::value(((Env*)env)->standard_error_);
    auto tag = core::Exception::tag(exception);
    auto source = core::Exception::source(exception);
    auto reason = core::Exception::reason(exception);
    auto frame = core::Exception::frame(exception);

    core::PrintStdString((Env*)env, "exception: ", std_error, false);
    core::Print((Env*)env, tag, std_error, false);
    core::PrintStdString((Env*)env, " ", std_error, false);
    core::Print((Env*)env, source, std_error, false);
    core::PrintStdString((Env*)env, " ", std_error, false);
    core::Print((Env*)env, reason, std_error, false);
    core::PrintStdString((Env*)env, " on frame ", std_error, false);
    core::Print((Env*)env, frame, std_error, false);
    core::Terpri((Env*)env, std_error);
  } catch (...) {
    printf("unexpected throw from library");
    exit(-1);
  }
}

/** * eval **/
void* eval(void* env, void* form) {
  return reinterpret_cast<void*>(core::Eval(
      (Env*)env,
      core::Compile((Env*)env, Type::Entag(form, Type::TAG::ADDRESS))));
}

/** * env - allocate an environment **/
void* env_default(Platform* platform) {
  auto stdin = Platform::OpenStandardStream(Platform::STD_STREAM::STDIN);
  auto stdout = Platform::OpenStandardStream(Platform::STD_STREAM::STDOUT);
  auto stderr = Platform::OpenStandardStream(Platform::STD_STREAM::STDERR);

  return (void*)new Env(platform, stdin, stdout, stderr);
}

/** * env - allocate an environment **/
void* env(Platform* platform, Platform::StreamId stdin,
          Platform::StreamId stdout, Platform::StreamId stderr) {
  return (void*)new Env(platform, stdin, stdout, stderr);
}

} /* namespace api */
} /* namespace libmu */
