/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  cons.h: library conses
 **
 **/
#if !defined(LIBMU_TYPES_CONS_H_)
#define LIBMU_TYPES_CONS_H_

#include <cassert>
#include <functional>
#include <memory>

#include "libmu/type.h"

namespace libmu {
namespace core {

using Tag = core::Type::Tag;

/** * cons type class **/
class Cons : public Type {
 private:
  typedef struct {
    Tag car;
    Tag cdr;
  } HeapLayout;

  HeapLayout cons_;

 public: /* Tag */
  static constexpr auto car(Tag cp) -> Tag {
    assert(IsList(cp));

    return Null(cp) ? NIL : Untag<HeapLayout>(cp)->car;
  }

  static constexpr auto cdr(Tag cp) -> Tag {
    assert(IsList(cp));

    return Null(cp) ? NIL : Untag<HeapLayout>(cp)->cdr;
  }

  static constexpr auto IsType(Tag ptr) -> bool {
    return TagOf(ptr) == TAG::CONS;
  }
  static constexpr auto IsList(Tag ptr) -> bool {
    return IsType(ptr) || Null(ptr);
  }

  static auto ListToVec(Tag, std::vector<Tag>&) -> void;
  static auto List(Env*, const std::vector<Tag>&) -> Tag;
  static auto ListDot(Env*, const std::vector<Tag>&) -> Tag;

  static auto Nth(Tag, size_t) -> Tag;
  static auto NthCdr(Tag, size_t) -> Tag;
  static auto Length(Env*, Tag) -> size_t;

  static auto MapC(Env*, Tag, Tag) -> void;
  static auto MapCar(Env*, Tag, Tag) -> Tag;
  static auto MapL(Env*, Tag, Tag) -> void;
  static auto MapList(Env*, Tag, Tag) -> Tag;

  static auto Print(Env*, Tag, Tag, bool) -> void;
  static auto Read(Env*, Tag) -> Tag;

  static auto GcMark(Env*, Tag) -> void;
  static auto ViewOf(Env*, Tag) -> Tag;

 public: /* type model */
  auto Evict(Env*) -> Tag;
  auto Evict(Env*, Tag) -> Tag;

 public: /* object */
  explicit Cons(Tag, Tag);

 public:
  /** * cons iterator **/
  template <typename T>
  struct cons_iter {
    typedef HeapLayout* iterator;

    iterator cons_;
    iterator current_;

    cons_iter(Tag cons)
        : cons_(Null(cons) ? end() : Untag<HeapLayout>(cons)), current_(cons_) {
      assert(IsList(cons));
    }

    iterator begin() { return cons_; }

    iterator end() { return nullptr; }

    bool operator!=(const struct cons_iter& iter) {
      return current_ != iter.current_;
    }

    iterator& operator++() { /* ++operator */

      current_ =
          IsType(current_->cdr) ? Untag<HeapLayout>(current_->cdr) : end();
      return *&current_;
    }

    iterator operator++(int) { /* operator++ */

      iterator retn = current_;
      operator++();

      return retn;
    }

    Tag operator*() { return current_->car; }
  };

}; /* class Cons */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TYPES_CONS_H_ */
