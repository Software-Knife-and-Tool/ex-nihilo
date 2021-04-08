/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  condition.cc: libmu conditions
 **
 **/
#include "libmu/types/condition.h"

#include <cassert>
#include <iostream>
#include <map>

#include "libmu/core.h"
#include "libmu/env.h"
#include "libmu/tagformat.h"
#include "libmu/type.h"

#include "libmu/types/function.h"
#include "libmu/types/namespace.h"

namespace libmu {
namespace core {

using Tag = Type::Tag;

/** * condition vector to heap **/
auto Condition::Evict(Env* env) -> Tag {
  auto hp = env->heap_alloc<Layout>(sizeof(Layout), SYS_CLASS::CONDITION);

  *hp = condition_;
  hp->tag = Env::Evict(env, condition_.tag);
  hp->frame = Env::Evict(env, condition_.frame);
  hp->source = Env::Evict(env, condition_.source);
  hp->reason = Env::Evict(env, condition_.reason);

  tag_ = Entag(hp, TAG::EXTEND);

  return tag_;
}

auto Condition::EvictTag(Env* env, Tag cond) -> Tag {
  assert(IsType(cond));
  assert(!Env::IsEvicted(env, cond));

  Layout* hi = Untag<Layout>(cond);
  auto hp = env->heap_alloc<Layout>(sizeof(Layout), SYS_CLASS::CONDITION);

  *hp = *hi;

  hp->tag = Env::Evict(env, hi->tag);
  hp->frame = Env::Evict(env, hi->frame);
  hp->source = Env::Evict(env, hi->source);
  hp->reason = Env::Evict(env, hi->reason);

  return Entag(hp, TAG::EXTEND);
}

/** * garbage collection **/
auto Condition::GcMark(Env* env, Tag condition) -> void {
  assert(IsType(condition));

  if (!TagFormat<Layout>::IsGcMarked(condition)) {
    TagFormat<Layout>::GcMark(condition);
    env->GcMark(env, tag(condition));
    env->GcMark(env, source(condition));
    env->GcMark(env, frame(condition));
    env->GcMark(env, reason(condition));
  }
}

/** * make view of condition **/
auto Condition::ViewOf(Env* env, Tag ex) -> Tag {
  assert(IsType(ex));

  auto view = std::vector<Tag>{Symbol::Keyword("cond"),
                               ex,
                               Fixnum(ToUint64(ex) >> 3).tag_,
                               tag(ex),
                               source(ex),
                               reason(ex),
                               frame(ex)};

  return Vector(env, view).tag_;
}

/** * raise condition **/
auto Condition::Raise(Env* env, CONDITION_CLASS ctype,
                      const std::string& reason, Tag source) -> void {
  typedef std::pair<std::string, Tag> ExDesc;

  static const std::map<CONDITION_CLASS, ExDesc> kExceptMap{
      {CONDITION_CLASS::SIMPLE_ERROR,
       {"simple-error", Symbol::Keyword("simple")}},
      {CONDITION_CLASS::ARITHMETIC_ERROR,
       {"arithmetic-error", Symbol::Keyword("arith")}},
      {CONDITION_CLASS::FLOATING_POINT_OVERFLOW,
       {"floating-point-overflow", Symbol::Keyword("fpover")}},
      {CONDITION_CLASS::CELL_ERROR, {"cell-error", Symbol::Keyword("cell")}},
      {CONDITION_CLASS::FLOATING_POINT_UNDERFLOW,
       {"floating-point-underflow", Symbol::Keyword("fpunder")}},
      {CONDITION_CLASS::STORAGE_CONDITION,
       {"storage-condition", Symbol::Keyword("store")}},
      {CONDITION_CLASS::CONTROL_ERROR,
       {"control-error", Symbol::Keyword("control")}},
      {CONDITION_CLASS::PARSE_ERROR, {"parse-error", Symbol::Keyword("parse")}},
      {CONDITION_CLASS::STREAM_ERROR,
       {"stream-error", Symbol::Keyword("stream")}},
      {CONDITION_CLASS::DIVISION_BY_ZERO,
       {"division-by-zero", Symbol::Keyword("zerodiv")}},
      {CONDITION_CLASS::PRINT_NOT_READABLE,
       {"print-not-readable", Symbol::Keyword("print")}},
      {CONDITION_CLASS::END_OF_FILE, {"end-of-file", Symbol::Keyword("eof")}},
      {CONDITION_CLASS::PROGRAM_ERROR,
       {"program-error", Symbol::Keyword("program")}},
      {CONDITION_CLASS::TYPE_ERROR, {"type-error", Symbol::Keyword("type")}},
      {CONDITION_CLASS::READER_ERROR,
       {"reader-error", Symbol::Keyword("read")}},
      {CONDITION_CLASS::UNBOUND_SLOT,
       {"unbound-slot", Symbol::Keyword("unslot")}},
      {CONDITION_CLASS::FILE_ERROR, {"file-error", Symbol::Keyword("file")}},
      {CONDITION_CLASS::UNBOUND_VARIABLE,
       {"unbound-variable", Symbol::Keyword("unsym")}},
      {CONDITION_CLASS::FLOATING_POINT_INEXACT,
       {"floating-point-inexact", Symbol::Keyword("fpinex")}},
      {CONDITION_CLASS::UNDEFINED_FUNCTION,
       {"undefined-function", Symbol::Keyword("unfunc")}},
      {CONDITION_CLASS::FLOATING_POINT_INVALID_OPERATION,
       {"floating-point-invalid-operation", Symbol::Keyword("fpinv")}}};

  assert(kExceptMap.count(ctype) != 0);

  auto ex = kExceptMap.at(ctype);

  throw Condition(ex.second, Env::LastFrame(env), source,
                  String(env, reason).tag_)
      .Evict(env);
}

} /* namespace core */
} /* namespace libmu */
