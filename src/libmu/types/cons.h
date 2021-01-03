/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
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
  } Layout;

  Layout cons_;

 public: /* Tag */
  static constexpr Tag car(Tag cp) {
    assert(IsList(cp));

    return Null(cp) ? NIL : Untag<Layout>(cp)->car;
  }

  static constexpr Tag cdr(Tag cp) {
    assert(IsList(cp));

    return Null(cp) ? NIL : Untag<Layout>(cp)->cdr;
  }

  static constexpr bool IsType(Tag ptr) { return TagOf(ptr) == TAG::CONS; }
  static constexpr bool IsList(Tag ptr) { return IsType(ptr) || Null(ptr); }

  static void ListToVec(Tag, std::vector<Tag>&);
  static Tag List(Env*, const std::vector<Tag>&);
  static Tag ListDot(Env*, const std::vector<Tag>&);

  static Tag Nth(Tag, size_t);
  static Tag NthCdr(Tag, size_t);
  static size_t Length(Env*, Tag);

  static void MapC(Env*, Tag, Tag);
  static Tag MapCar(Env*, Tag, Tag);
  static void MapL(Env*, Tag, Tag);
  static Tag MapList(Env*, Tag, Tag);

  static void Print(Env*, Tag, Tag, bool);
  static Tag Read(Env*, Tag);

  static void GcMark(Env*, Tag);
  static Tag ViewOf(Env*, Tag);

 public: /* object */
  Tag Evict(Env*);

  explicit Cons(Tag, Tag);

 public:
  /** * cons iterator **/
  template <typename T>
  struct cons_iter {
    typedef Layout* iterator;

    iterator cons_;
    iterator current_;

    cons_iter(Tag cons)
        : cons_(Null(cons) ? end() : Untag<Layout>(cons)), current_(cons_) {
      assert(IsList(cons));
    }

    iterator begin() { return cons_; }

    iterator end() { return nullptr; }

    bool operator!=(const struct cons_iter& iter) {
      return current_ != iter.current_;
    }

    iterator& operator++() { /* ++operator */

      current_ = IsType(current_->cdr) ? Untag<Layout>(current_->cdr) : end();
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
