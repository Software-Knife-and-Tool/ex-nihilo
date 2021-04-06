/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  libmu.cc: library API
 **
 **/
#include "libmu/libmu.h"

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <memory>

#include "libmu/platform/platform.h"

#include "libmu/compiler.h"
#include "libmu/core.h"
#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/heap/heap.h"

#include "libmu/types/condition.h"
#include "libmu/types/namespace.h"
#include "libmu/types/stream.h"
#include "libmu/types/string.h"
#include "libmu/types/symbol.h"

namespace libmu {
namespace api {

using Type = core::Type;
using Env = core::Env;

/** * constants interface **/
auto t() -> uintptr_t { return static_cast<uintptr_t>(Type::T); }
auto nil() -> uintptr_t { return static_cast<uintptr_t>(Type::NIL); }
auto version() -> const char* { return "0.0.23"; }

/** * read interface **/
auto read_stream(void* env, uintptr_t stream) -> uintptr_t {
  return static_cast<uintptr_t>(
      core::Read((Env*)env, static_cast<Type::Tag>(stream)));
}

/** * read interface **/
auto read_cstr(void* env, const char* src) -> uintptr_t {
  return static_cast<uintptr_t>(
      core::Read((Env*)env, core::Stream::MakeInputString((Env*)env, src)));
}

/** * read interface **/
auto read_string(void* env, const std::string& src) -> uintptr_t {
  return read_cstr(env, src.c_str());
}

/** * princ/prin1 interface **/
auto print(void* env, uintptr_t ptr, uintptr_t stream, bool esc) -> void {
  core::Print((Env*)env, static_cast<Type::Tag>(ptr),
              static_cast<Type::Tag>(stream), esc);
}

/** * print to a cstring **/
auto print_cstr(void* env, uintptr_t ptr, bool esc) -> const char* {
  auto stream = core::Stream::MakeOutputString((Env*)env, "");

  core::Print((Env*)env, static_cast<Type::Tag>(ptr),
              static_cast<Type::Tag>(stream), esc);

  auto cp = Platform::GetStdString(core::Stream::streamId(stream));
  auto cstr = new char[strlen(cp.c_str()) + 1];

  return strcpy(cstr, cp.c_str());
}

/** * print end of line **/
auto terpri(void* env, uintptr_t stream) -> void {
  core::Terpri((Env*)env, static_cast<Type::Tag>(stream));
}

/** * catch library throws **/
auto withCondition(void* env, const std::function<void(void*)>& fn) -> void {
  auto ev = reinterpret_cast<Env*>(env);
  auto mark = ev->frames_.size();

  try {
    fn(env);
  } catch (Type::Tag condition) {
    auto std_error = core::Symbol::value(((Env*)env)->standard_error_);
    auto tag = core::Condition::tag(condition);
    auto source = core::Condition::source(condition);
    auto reason = core::Condition::reason(condition);
    auto frame = core::Condition::frame(condition);

    core::PrintStdString((Env*)env, "condition: ", std_error, false);
    core::Print((Env*)env, tag, std_error, true);
    core::PrintStdString((Env*)env, " ", std_error, false);
    core::Print((Env*)env, source, std_error, true);
    core::PrintStdString((Env*)env, " ", std_error, false);
    core::Print((Env*)env, reason, std_error, false);
    core::PrintStdString((Env*)env, " on frame ", std_error, false);
    core::Print((Env*)env, frame, std_error, false);
    core::Terpri((Env*)env, std_error);

    core::PrintStdString((Env*)env, "source: ", std_error, false);
    core::Print((Env*)env, ev->src_form_, std_error, true);
    core::Terpri((Env*)env, std_error);

    ev->frames_.resize(mark);

  } catch (...) {
    printf("unexpected throw from library");
    exit(-1);
  }
}

/** * eval **/
auto eval(void* env, uintptr_t form) -> uintptr_t {
  return static_cast<uintptr_t>(core::Eval(
      (Env*)env, core::Compile((Env*)env, static_cast<Type::Tag>(form))));
}

/** * env - allocate an environment **/
auto env_default(Platform* platform) -> uintptr_t {
  auto stdin = Platform::OpenStandardStream(Platform::STD_STREAM::STDIN);
  auto stdout = Platform::OpenStandardStream(Platform::STD_STREAM::STDOUT);
  auto stderr = Platform::OpenStandardStream(Platform::STD_STREAM::STDERR);

  return (uintptr_t) new Env(platform, stdin, stdout, stderr);
}

/** * env - allocate an environment **/
auto env(Platform* platform, Platform::StreamId stdin,
         Platform::StreamId stdout, Platform::StreamId stderr) -> uintptr_t {
  return (uintptr_t) new Env(platform, stdin, stdout, stderr);
}

} /* namespace api */
} /* namespace libmu */
