/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  cons.cc: library conses
 **
 **/
#include "libmu/types/cons.h"

#include <cassert>

#include "libmu/print.h"
#include "libmu/read.h"
#include "libmu/type.h"

#include "libmu/types/exception.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/function.h"
#include "libmu/types/stream.h"

namespace libmu {
namespace core {

/** * garbage collection **/
void Cons::GcMark(Env* env, TagPtr ptr) {
  assert(IsType(ptr));

  if (!env->heap_->IsGcMarked(ptr)) {
    env->heap_->GcMark(ptr);
    env->GcMark(env, car(ptr));
    env->GcMark(env, cdr(ptr));
  }
}

/** * view of cons object **/
TagPtr Cons::ViewOf(Env* env, TagPtr cons) {
  assert(IsType(cons));

  auto view = std::vector<TagPtr>{Symbol::Keyword("cons"), cons,
                                  Fixnum(ToUint64(cons) >> 3).tag_, car(cons),
                                  cdr(cons)};

  return Vector(env, view).tag_;
}

/** * listToVec list vector ref **/
void Cons::ListToVec(TagPtr list, std::vector<TagPtr>& vec) {
  assert(IsList(list));

  cons_iter<TagPtr> iter(list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    vec.push_back(it->car);
}

/** * mapcar :func list **/
TagPtr Cons::MapCar(Env* env, TagPtr func, TagPtr list) {
  assert(Function::IsType(func));
  assert(IsList(list));

  if (Null(list)) return NIL;

  std::vector<TagPtr> vlist;

  cons_iter<TagPtr> iter(list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    vlist.push_back(Function::Funcall(env, func, std::vector<TagPtr>{it->car}));

  return Cons::List(env, vlist);
}

/** * mapc function list **/
void Cons::MapC(Env* env, TagPtr func, TagPtr list) {
  assert(Function::IsType(func));
  assert(IsList(list));

  if (Null(list)) return;

  cons_iter<TagPtr> iter(list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    (void)Function::Funcall(env, func, std::vector<TagPtr>{it->car});
}

/** * maplist function list **/
TagPtr Cons::MapList(Env* env, TagPtr func, TagPtr list) {
  assert(Function::IsType(func));
  assert(IsList(list));

  if (Null(list)) return NIL;

  std::vector<TagPtr> vlist;
  cons_iter<TagPtr> iter(list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    vlist.push_back(Function::Funcall(
        env, func, std::vector<TagPtr>{Type::Entag(it, TAG::CONS)}));

  return Cons::List(env, vlist);
}

/** * maplist :func list **/
void Cons::MapL(Env* env, TagPtr func, TagPtr list) {
  assert(Function::IsType(func));
  assert(IsList(list));

  if (Null(list)) return;

  cons_iter<TagPtr> iter(list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    (void)Function::Funcall(env, func,
                            std::vector<TagPtr>{Type::Entag(it, TAG::CONS)});
}

/** * make a list from a std::vector **/
TagPtr Cons::List(Env* env, const std::vector<TagPtr>& src) {
  if (src.size() == 0) return NIL;

  TagPtr rlist = NIL;

  for (auto nth = src.size(); nth; --nth)
    rlist = Cons(src[nth - 1], rlist).Evict(env);

  return rlist;
}

/** * make a dotted list from a std::vector<TagPtr> **/
TagPtr Cons::ListDot(Env* env, const std::vector<TagPtr>& src) {
  if (src.size() == 0) return NIL;

  TagPtr rlist = Cons(src[src.size() - 2], src[src.size() - 1]).Evict(env);

  if (src.size() > 2)
    for (auto nth = src.size() - 2; nth != 0; --nth)
      rlist = Cons(src[nth - 1], rlist).Evict(env);

  return rlist;
}

/** * nth element of list **/
TagPtr Cons::Nth(TagPtr list, size_t index) {
  assert(IsList(list));

  TagPtr nth = NIL;

  cons_iter<TagPtr> iter(list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
    nth = it->car;
    if (index-- == 0) break;
  }

  return nth;
}

/** * nth sublist of list **/
TagPtr Cons::NthCdr(TagPtr list, size_t index) {
  assert(IsList(list));

  if (index == 0) return list;

  cons_iter<TagPtr> iter(list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    if (--index == 0) return it->cdr;

  return NIL;
}

/** * list length **/
size_t Cons::Length(Env* env, TagPtr list) {
  assert(IsList(list));

  if (Null(list)) return 0;

  size_t len = 0;

  cons_iter<TagPtr> iter(list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
    len++;
    if (!IsList(it->cdr))
      Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                       "is not a list (list-length)", list);
  }

  return len;
}

/** * print list to stream **/
void Cons::Print(Env* env, TagPtr cons, TagPtr stream, bool esc) {
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
TagPtr Cons::Read(Env* env, TagPtr stream) {
  assert(Stream::IsType(stream));

  std::vector<TagPtr> vlist;
  TagPtr ch;

  if (!core::ReadWSUntilEof(env, stream))
    Exception::Raise(env, Exception::EXCEPT_CLASS::PARSE_ERROR,
                     "early end of file in list form (read)", Type::NIL);

  ch = Stream::ReadByte(env, stream);
  if (core::MapSyntaxChar(ch) == core::SYNTAX_CHAR::CPAREN) return Type::NIL;

  Stream::UnReadByte(ch, stream);

  for (;;) {
    auto el = core::ReadForm(env, stream);

    if (core::SyntaxEq(el, core::SYNTAX_CHAR::DOT)) {
      vlist.push_back(core::ReadForm(env, stream));

      if (!core::ReadWSUntilEof(env, stream))
        Exception::Raise(env, Exception::EXCEPT_CLASS::PARSE_ERROR,
                         "early end of file in dotted form", Type::NIL);

      ch = Stream::ReadByte(env, stream);
      if (core::MapSyntaxChar(ch) != core::SYNTAX_CHAR::CPAREN)
        Exception::Raise(env, Exception::EXCEPT_CLASS::PARSE_ERROR,
                         "syntax problem in dotted form (read)",
                         Char(Fixnum::Uint64Of(ch)).tag_);

      return Cons::ListDot(env, vlist);
    }

    vlist.push_back(el);

    if (!core::ReadWSUntilEof(env, stream))
      Exception::Raise(env, Exception::EXCEPT_CLASS::PARSE_ERROR,
                       "early end of file in list form (read)", Type::NIL);

    ch = Stream::ReadByte(env, stream);
    if (core::MapSyntaxChar(ch) == core::SYNTAX_CHAR::CPAREN)
      return Cons::List(env, vlist);

    Stream::UnReadByte(ch, stream);
  }
}

/** * evict cons to heap **/
TagPtr Cons::Evict(Env* env) {
  auto cp = env->heap_alloc<Layout>(sizeof(Layout), SYS_CLASS::CONS);

  *cp = cons_;
  tag_ = Entag(cp, TAG::CONS);

  return tag_;
}

/** * allocate cons from the heap **/
Cons::Cons(TagPtr car, TagPtr cdr) : Type() {
  cons_.car = car;
  cons_.cdr = cdr;

  tag_ = Entag(reinterpret_cast<void*>(&cons_), TAG::CONS);
}

} /* namespace core */
} /* namespace libmu */
