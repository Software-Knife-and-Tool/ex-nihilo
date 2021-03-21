/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  cons.cc: library conses
 **
 **/
#include "libmu/types/cons.h"

#include <cassert>

#include "libmu/core.h"
#include "libmu/type.h"

#include "libmu/types/condition.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/function.h"
#include "libmu/types/stream.h"

namespace libmu {
namespace core {

/** * garbage collection **/
auto Cons::GcMark(Env* env, Tag ptr) -> void {
  assert(IsType(ptr));

  if (!env->heap_->IsGcMarked(ptr)) {
    env->heap_->GcMark(ptr);
    env->GcMark(env, car(ptr));
    env->GcMark(env, cdr(ptr));
  }
}

/** * view of cons object **/
auto Cons::ViewOf(Env* env, Tag cons) -> Tag {
  assert(IsType(cons));

  auto view =
      std::vector<Tag>{Symbol::Keyword("cons"), cons,
                       Fixnum(ToUint64(cons) >> 3).tag_, car(cons), cdr(cons)};

  return Vector(env, view).tag_;
}

/** * listToVec list to vector **/
auto Cons::ListToVec(Tag list, std::vector<Tag>& vec) -> void {
  assert(IsList(list));

  cons_iter<Tag> iter(list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    vec.push_back(it->car);
}

/** * mapcar :func list **/
auto Cons::MapCar(Env* env, Tag func, Tag list) -> Tag {
  assert(Function::IsType(func));
  assert(IsList(list));

  if (Null(list)) return NIL;

  std::vector<Tag> vlist;

  cons_iter<Tag> iter(list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    vlist.push_back(Function::Funcall(env, func, std::vector<Tag>{it->car}));

  return Cons::List(env, vlist);
}

/** * mapc function list **/
auto Cons::MapC(Env* env, Tag func, Tag list) -> void {
  assert(Function::IsType(func));
  assert(IsList(list));

  if (Null(list)) return;

  cons_iter<Tag> iter(list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    (void)Function::Funcall(env, func, std::vector<Tag>{it->car});
}

/** * maplist function list **/
auto Cons::MapList(Env* env, Tag func, Tag list) -> Tag {
  assert(Function::IsType(func));
  assert(IsList(list));

  if (Null(list)) return NIL;

  std::vector<Tag> vlist;
  cons_iter<Tag> iter(list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    vlist.push_back(Function::Funcall(
        env, func, std::vector<Tag>{Type::Entag(it, TAG::CONS)}));

  return Cons::List(env, vlist);
}

/** * maplist :func list **/
auto Cons::MapL(Env* env, Tag func, Tag list) -> void {
  assert(Function::IsType(func));
  assert(IsList(list));

  if (Null(list)) return;

  cons_iter<Tag> iter(list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    (void)Function::Funcall(env, func,
                            std::vector<Tag>{Type::Entag(it, TAG::CONS)});
}

/** * make a list from a std::vector **/
auto Cons::List(Env* env, const std::vector<Tag>& src) -> Tag {
  if (src.size() == 0) return NIL;

  Tag rlist = NIL;

  for (auto nth = src.size(); nth; --nth)
    rlist = Cons(src[nth - 1], rlist).Evict(env);

  return rlist;
}

/** * make a dotted list from a std::vector<Tag> **/
auto Cons::ListDot(Env* env, const std::vector<Tag>& src) -> Tag {
  if (src.size() == 0) return NIL;

  Tag rlist = Cons(src[src.size() - 2], src[src.size() - 1]).Evict(env);

  if (src.size() > 2)
    for (auto nth = src.size() - 2; nth != 0; --nth)
      rlist = Cons(src[nth - 1], rlist).Evict(env);

  return rlist;
}

/** * nth element of list **/
auto Cons::Nth(Tag list, size_t index) -> Tag {
  assert(IsList(list));

  Tag nth = NIL;

  cons_iter<Tag> iter(list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
    nth = it->car;
    if (index-- == 0) break;
  }

  return nth;
}

/** * nth sublist of list **/
auto Cons::NthCdr(Tag list, size_t index) -> Tag {
  assert(IsList(list));

  if (index == 0) return list;

  cons_iter<Tag> iter(list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    if (--index == 0) return it->cdr;

  return NIL;
}

/** * list length **/
auto Cons::Length(Env* env, Tag list) -> size_t {
  assert(IsList(list));

  if (Null(list)) return 0;

  size_t len = 0;

  cons_iter<Tag> iter(list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
    len++;
    if (!IsList(it->cdr))
      Condition::Raise(env, Condition::CONDITION_CLASS::TYPE_ERROR,
                       "is not a list (length)", list);
  }

  return len;
}

/** * print list to stream **/
auto Cons::Print(Env* env, Tag cons, Tag stream, bool esc) -> void {
  assert(IsList(cons));
  assert(Stream::IsType(stream));

  core::PrintStdString(env, "(", stream, false);

  auto lp = cons;

  for (; Cons::IsType(lp) && !Null(lp); lp = Cons::cdr(lp)) {
    if (!Eq(lp, cons)) core::PrintStdString(env, " ", stream, false);
    core::Print(env, Cons::car(lp), stream, esc);
  }

  if (!IsList(lp)) {
    core::PrintStdString(env, " . ", stream, false);
    core::Print(env, lp, stream, esc);
  }

  core::PrintStdString(env, ")", stream, false);
}

/** * list parser **/
auto Cons::Read(Env* env, Tag stream) -> Tag {
  assert(Stream::IsType(stream));

  std::vector<Tag> vlist;
  Tag ch;

  if (!core::ReadWSUntilEof(env, stream))
    Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                     "early end of file in list form (read)", Type::NIL);

  ch = Stream::ReadByte(env, stream);
  if (core::MapSyntaxChar(ch) == core::SYNTAX_CHAR::CPAREN) return Type::NIL;

  Stream::UnReadByte(ch, stream);

  for (;;) {
    auto el = core::ReadForm(env, stream);

    if (core::SyntaxEq(el, core::SYNTAX_CHAR::DOT)) {
      vlist.push_back(core::ReadForm(env, stream));

      if (!core::ReadWSUntilEof(env, stream))
        Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                         "early end of file in dotted form", Type::NIL);

      ch = Stream::ReadByte(env, stream);
      if (core::MapSyntaxChar(ch) != core::SYNTAX_CHAR::CPAREN)
        Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                         "syntax problem in dotted form (read)",
                         Char(Fixnum::Uint64Of(ch)).tag_);

      return Cons::ListDot(env, vlist);
    }

    vlist.push_back(el);

    if (!core::ReadWSUntilEof(env, stream))
      Condition::Raise(env, Condition::CONDITION_CLASS::PARSE_ERROR,
                       "early end of file in list form (read)", Type::NIL);

    ch = Stream::ReadByte(env, stream);
    if (core::MapSyntaxChar(ch) == core::SYNTAX_CHAR::CPAREN)
      return Cons::List(env, vlist);

    Stream::UnReadByte(ch, stream);
  }
}

/** * evict cons to heap **/
auto Cons::Evict(Env* env) -> Tag {
  auto cp = env->heap_alloc<HeapLayout>(sizeof(HeapLayout), SYS_CLASS::CONS);

  *cp = cons_;
  tag_ = Entag(cp, TAG::CONS);

  return tag_;
}

auto Cons::EvictTag(Env* env, Tag cons) -> Tag {
  assert(IsType(cons));
  assert(!Env::IsEvicted(env, cons));

  return Cons(car(cons), cdr(cons)).Evict(env);
}

/** * allocate cons **/
Cons::Cons(Tag car, Tag cdr) : Type() {
  cons_.car = car;
  cons_.cdr = cdr;

  tag_ = Entag(reinterpret_cast<void*>(&cons_), TAG::CONS);
}

} /* namespace core */
} /* namespace libmu */
