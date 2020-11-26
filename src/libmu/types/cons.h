/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  cons.h: library conses
 **
 **/
#if !defined(_LIBMU_TYPES_CONS_H_)
#define _LIBMU_TYPES_CONS_H_

#include <cassert>
#include <functional>
#include <memory>

#include "libmu/type.h"

namespace libmu {

/** * cons type class **/
class Cons : public Type {
 private:
  typedef struct {
    TagPtr car;
    TagPtr cdr;
  } Layout;

  Layout cons_;

 public: /* TagPtr */
  static constexpr TagPtr car(TagPtr cp) {
    assert(IsList(cp));

    return Null(cp) ? NIL : Untag<Layout>(cp)->car;
  }

  static constexpr TagPtr cdr(TagPtr cp) {
    assert(IsList(cp));

    return Null(cp) ? NIL : Untag<Layout>(cp)->cdr;
  }

  static constexpr bool IsType(TagPtr ptr) { return TagOf(ptr) == TAG::CONS; }
  static constexpr bool IsList(TagPtr ptr) { return IsType(ptr) || Null(ptr); }

  static TagPtr List(Env*, const std::vector<TagPtr>&);
  static TagPtr ListDot(Env*, const std::vector<TagPtr>&);

  static TagPtr Nth(TagPtr, size_t);
  static TagPtr NthCdr(TagPtr, size_t);
  static size_t Length(Env*, TagPtr);

  static void MapC(Env*, std::function<void(Env*, TagPtr)>, TagPtr);
  static void MapC(Env*, TagPtr, TagPtr);

  static TagPtr MapCar(Env*, std::function<TagPtr(Env*, TagPtr)>, TagPtr);
  static TagPtr MapCar(Env*, TagPtr, TagPtr);

  static void MapL(Env*, TagPtr, TagPtr);
  static TagPtr MapList(Env*, TagPtr, TagPtr);

  static void PrintCons(Env*, TagPtr, TagPtr, bool);
  static TagPtr Read(Env*, TagPtr);

  static void GcMark(Env*, TagPtr);
  static TagPtr ViewOf(Env*, TagPtr);

 public: /* object */
  TagPtr Evict(Env*, const char*);

  explicit Cons(TagPtr, TagPtr);
  
 public:
  /** * cons iterator **/
  template <typename T>
  struct cons_iter {
    typedef Layout* iterator;

    iterator cons_;
    iterator current_;

    cons_iter(TagPtr cons)
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

    TagPtr operator*() { return current_->car; }
  };

}; /* class Cons */

} /* namespace libmu */

#endif /* _LIBMU_TYPES_CONS_H_ */
