/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  exception.h: libmu exceptions
 **
 **/
#if !defined(_LIBMU_TYPES_EXCEPTION_H_)
#define _LIBMU_TYPES_EXCEPTION_H_

#include <cassert>

#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/heap/heap.h"

#include "libmu/types/cons.h"

namespace libmu {

/** * exception type class **/
class Exception : public Type {
 public:
  enum class EXCEPT_CLASS : uint8_t {
    ARITHMETIC_ERROR,
    CELL_ERROR,
    CONTROL_ERROR,
    DIVISION_BY_ZERO,
    END_OF_FILE,
    FILE_ERROR,
    FLOATING_POINT_INEXACT,
    FLOATING_POINT_INVALID_OPERATION,
    FLOATING_POINT_OVERFLOW,
    FLOATING_POINT_UNDERFLOW,
    PARSE_ERROR,
    PRINT_NOT_READABLE,
    PROGRAM_ERROR,
    RANGE_ERROR,
    READER_ERROR,
    SIMPLE_ERROR,
    STORAGE_CONDITION,
    STREAM_ERROR,
    TYPE_ERROR,
    UNBOUND_SLOT,
    UNBOUND_VARIABLE,
    UNDEFINED_FUNCTION
  };

 private:
  typedef struct {
    TagPtr tag;    /* exception tag */
    TagPtr frame;  /* frame */
    TagPtr source; /* on source object */
    TagPtr reason; /* exception string */
  } Layout;

  Layout exception_;

 public:
  static TagPtr frame(TagPtr exception) {
    assert(IsType(exception));

    return Untag<Layout>(exception)->frame;
  }

  static TagPtr tag(TagPtr exception) {
    assert(IsType(exception));

    return Untag<Layout>(exception)->tag;
  }

  static TagPtr source(TagPtr exception) {
    assert(IsType(exception));

    return Untag<Layout>(exception)->source;
  }

  static TagPtr reason(TagPtr exception) {
    assert(IsType(exception));

    return Untag<Layout>(exception)->reason;
  }

  static constexpr bool IsType(TagPtr ptr) {
    return IsExtended(ptr) && Heap::SysClass(ptr) == SYS_CLASS::EXCEPTION;
  }

  static void GcMark(Env*, TagPtr);

  [[noreturn]] static void Raise(Env*, EXCEPT_CLASS, const std::string&,
                                 TagPtr);
  static TagPtr ViewOf(Env*, TagPtr);

 public: /* object model */
  TagPtr Evict(Env* env, const char* src) {
    auto ep =
        env->heap_alloc<Layout>(sizeof(Layout), SYS_CLASS::EXCEPTION, src);

    *ep = exception_;
    return Entag(ep, TAG::EXTEND);
  }

  explicit Exception(TagPtr tag, TagPtr frame, TagPtr source, TagPtr reason)
      : Type() {
    assert(Symbol::IsKeyword(tag));

    exception_.tag = tag;
    exception_.frame = frame;
    exception_.source = source;
    exception_.reason = reason;

    tag_ = Entag(reinterpret_cast<void*>(&exception_), TAG::EXTEND);
  }
};

} /* namespace libmu */

#endif /* _LIBMU_TYPES_EXCEPTION_H_ */
