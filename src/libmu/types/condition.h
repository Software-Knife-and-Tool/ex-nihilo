/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  condition.h: libmu conditions
 **
 **/
#if !defined(LIBMU_TYPES_CONDITION_H_)
#define LIBMU_TYPES_CONDITION_H_

#include <cassert>

#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/heap/heap.h"
#include "libmu/types/cons.h"

namespace libmu {
namespace core {

/** * condition type class **/
class Condition : public Type {
 private:
  typedef struct {
    Tag tag;    /* condition tag */
    Tag frame;  /* frame */
    Tag source; /* on source object */
    Tag reason; /* condition string */
  } HeapLayout;

  HeapLayout condition_;

 public:
  enum class CONDITION_CLASS : uint8_t {
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

  static constexpr auto IsType(Tag ptr) -> bool {
    return IsExtended(ptr) && Heap::SysClass(ptr) == SYS_CLASS::CONDITION;
  }

  /** * accessors **/
  static auto frame(Tag condition) -> Tag {
    assert(IsType(condition));

    return Untag<HeapLayout>(condition)->frame;
  }

  static auto tag(Tag condition) -> Tag {
    assert(IsType(condition));

    return Untag<HeapLayout>(condition)->tag;
  }

  static auto source(Tag condition) -> Tag {
    assert(IsType(condition));

    return Untag<HeapLayout>(condition)->source;
  }

  static auto reason(Tag condition) -> Tag {
    assert(IsType(condition));

    return Untag<HeapLayout>(condition)->reason;
  }

 public: /* type model */
  auto Evict(Env* env) -> Tag {
    auto hp =
        env->heap_alloc<HeapLayout>(sizeof(HeapLayout), SYS_CLASS::CONDITION);

    *hp = condition_;

    hp->tag = Env::Evict(env, condition_.tag);
    hp->frame = Env::Evict(env, condition_.frame);
    hp->source = Env::Evict(env, condition_.source);
    hp->reason = Env::Evict(env, condition_.reason);

    tag_ = Entag(hp, TAG::EXTEND);

    return tag_;
  }

 public: /* object */
  static auto EvictTag(Env* env, Tag condition) -> Tag {
    assert(IsType(condition));
    assert(!Env::IsEvicted(env, condition));

    // printf("EvictTag: condition\n");

    auto hp =
        env->heap_alloc<HeapLayout>(sizeof(HeapLayout), SYS_CLASS::CONDITION);
    auto cp = Untag<HeapLayout>(condition);

    *hp = *cp;

    hp->tag = Env::Evict(env, cp->tag);
    hp->frame = Env::Evict(env, cp->frame);
    hp->source = Env::Evict(env, cp->source);
    hp->reason = Env::Evict(env, cp->reason);

    return Entag(hp, TAG::EXTEND);
  }

  static auto GcMark(Env*, Tag) -> void;
  static auto ViewOf(Tag) -> Tag;

  [[noreturn]] static auto Raise(Env*, CONDITION_CLASS, const std::string&, Tag)
      -> void;

  explicit Condition(Tag tag, Tag frame, Tag source, Tag reason) : Type() {
    assert(Symbol::IsKeyword(tag));

    condition_.tag = tag;
    condition_.frame = frame;
    condition_.source = source;
    condition_.reason = reason;

    tag_ = Entag(reinterpret_cast<void*>(&condition_), TAG::EXTEND);
  }
};

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TYPES_CONDITION_H_ */
