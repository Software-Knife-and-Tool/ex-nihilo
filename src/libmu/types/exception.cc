/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  exception.cc: libmu exceptions
 **
 **/
#include "libmu/types/exception.h"

#include <cassert>
#include <iostream>
#include <map>

#include "libmu/env.h"
#include "libmu/print.h"
#include "libmu/type.h"

#include "libmu/types/function.h"
#include "libmu/types/namespace.h"

namespace libmu {

using TagPtr = Type::TagPtr;

/** * garbage collection **/
void Exception::GcMark(Env* env, TagPtr exception) {
  assert(IsType(exception));

  if (!env->heap_->IsGcMarked(exception)) {
    env->heap_->GcMark(exception);
    env->GcMark(env, tag(exception));
    env->GcMark(env, source(exception));
    env->GcMark(env, frame(exception));
    env->GcMark(env, reason(exception));
  }
}

/** * make view of exception **/
Type::TagPtr Exception::ViewOf(Env* env, TagPtr ex) {
  assert(IsType(ex));

  auto view = std::vector<TagPtr>{Symbol::Keyword("except"),
                                  ex,
                                  Fixnum(ToUint64(ex) >> 3).tag_,
                                  tag(ex),
                                  source(ex),
                                  reason(ex),
                                  frame(ex)};

  return Vector(env, view).tag_;
}

/** * raise exception **/
void Exception::Raise(Env* env, EXCEPT_CLASS ctype, const std::string& reason,
                      TagPtr source) {
  typedef std::pair<std::string, TagPtr> ExDesc;

  static const std::map<EXCEPT_CLASS, ExDesc> kExceptMap{
      {EXCEPT_CLASS::SIMPLE_ERROR, {"simple-error", Symbol::Keyword("simple")}},
      {EXCEPT_CLASS::ARITHMETIC_ERROR,
       {"arithmetic-error", Symbol::Keyword("arith")}},
      {EXCEPT_CLASS::FLOATING_POINT_OVERFLOW,
       {"floating-point-overflow", Symbol::Keyword("fpover")}},
      {EXCEPT_CLASS::CELL_ERROR, {"cell-error", Symbol::Keyword("cell")}},
      {EXCEPT_CLASS::FLOATING_POINT_UNDERFLOW,
       {"floating-point-underflow", Symbol::Keyword("fpunder")}},
      {EXCEPT_CLASS::STORAGE_CONDITION,
       {"storage-condition", Symbol::Keyword("store")}},
      {EXCEPT_CLASS::CONTROL_ERROR,
       {"control-error", Symbol::Keyword("control")}},
      {EXCEPT_CLASS::PARSE_ERROR, {"parse-error", Symbol::Keyword("parse")}},
      {EXCEPT_CLASS::STREAM_ERROR, {"stream-error", Symbol::Keyword("stream")}},
      {EXCEPT_CLASS::DIVISION_BY_ZERO,
       {"division-by-zero", Symbol::Keyword("zerodiv")}},
      {EXCEPT_CLASS::PRINT_NOT_READABLE,
       {"print-not-readable", Symbol::Keyword("print")}},
      {EXCEPT_CLASS::END_OF_FILE, {"end-of-file", Symbol::Keyword("eof")}},
      {EXCEPT_CLASS::PROGRAM_ERROR,
       {"program-error", Symbol::Keyword("program")}},
      {EXCEPT_CLASS::TYPE_ERROR, {"type-error", Symbol::Keyword("type")}},
      {EXCEPT_CLASS::READER_ERROR, {"reader-error", Symbol::Keyword("read")}},
      {EXCEPT_CLASS::UNBOUND_SLOT, {"unbound-slot", Symbol::Keyword("unslot")}},
      {EXCEPT_CLASS::FILE_ERROR, {"file-error", Symbol::Keyword("file")}},
      {EXCEPT_CLASS::UNBOUND_VARIABLE,
       {"unbound-variable", Symbol::Keyword("unsym")}},
      {EXCEPT_CLASS::FLOATING_POINT_INEXACT,
       {"floating-point-inexact", Symbol::Keyword("fpinex")}},
      {EXCEPT_CLASS::UNDEFINED_FUNCTION,
       {"undefined-function", Symbol::Keyword("unfunc")}},
      {EXCEPT_CLASS::FLOATING_POINT_INVALID_OPERATION,
       {"floating-point-invalid-operation", Symbol::Keyword("fpinv")}}};

  assert(kExceptMap.count(ctype) != 0);

  auto ex = kExceptMap.at(ctype);

  throw Exception(ex.second, Env::LastFrame(env), source,
                  String(env, reason).tag_)
      .Evict(env, "exception:reason");
}

} /* namespace libmu */
