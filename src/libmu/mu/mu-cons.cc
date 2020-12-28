/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu-cons.cc: library list functions
 **
 **/
#include <cassert>

#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/types/cons.h"
#include "libmu/types/exception.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/float.h"
#include "libmu/types/function.h"
#include "libmu/types/vector.h"

namespace libmu {
namespace mu {

using Cons = core::Cons;
using Exception = core::Exception;
using Fixnum = core::Fixnum;
using Function = core::Function;
using Frame = core::Env::Frame;
using Type = core::Type;

/** * (consp vector) => generalized bool **/
void IsCons(Frame* fp) { fp->value = Type::Bool(Cons::IsType(fp->argv[0])); }

/** * (.mapcar function list) => list **/
void MapCar(Frame* fp) {
  auto func = fp->argv[0];
  auto list = fp->argv[1];

  if (!Function::IsType(func))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, ".mapcar",
                     func);

  if (!Cons::IsList(list))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, ".mapcar",
                     list);

  fp->value = Type::Null(list) ? Type::NIL : Cons::MapCar(fp->env, func, list);
}

/** * (mapc function list) => list **/
void MapC(Frame* fp) {
  auto func = fp->argv[0];
  auto list = fp->argv[1];

  if (!Function::IsType(func))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, ".mapc",
                     func);

  if (!Cons::IsList(list))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, ".mapc",
                     list);

  Cons::MapC(fp->env, func, list);
  fp->value = list;
}

/** * (maplist function list) => list **/
void MapList(Frame* fp) {
  auto func = fp->argv[0];
  auto list = fp->argv[1];

  if (!Function::IsType(func))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, ".maplist",
                     func);

  if (!Cons::IsList(list))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, ".maplist",
                     list);

  fp->value = Type::Null(list) ? Type::NIL : Cons::MapList(fp->env, func, list);
}

/** * (mapl function list)) => list **/
void MapL(Frame* fp) {
  auto func = fp->argv[0];
  auto list = fp->argv[1];

  if (!Function::IsType(func))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, ".mapl",
                     func);

  if (!Cons::IsList(list))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, ".mapl",
                     list);

  Cons::MapL(fp->env, func, list);
  fp->value = list;
}

/** * (list-length list) => fixnum**/
void ListLength(Frame* fp) {
  auto list = fp->argv[0];

  if (!Cons::IsList(list))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "list-length", list);

  fp->value = Fixnum(Cons::Length(fp->env, list)).tag_;
}

/** * (cons object object) => cons **/
void MakeCons(Frame* fp) {
  fp->value = Cons(fp->argv[0], fp->argv[1]).Evict(fp->env);
}

/** * (car list) => object **/
void Car(Frame* fp) {
  auto list = fp->argv[0];

  if (!Cons::IsList(list))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "car", list);

  fp->value = Cons::car(list);
}

/** *  (cdr list) => object **/
void Cdr(Frame* fp) {
  auto list = fp->argv[0];

  if (!Cons::IsList(list))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "cdr", list);

  fp->value = Cons::cdr(list);
}

/** * (nth fixnum list) => object **/
void Nth(Frame* fp) {
  auto nth = fp->argv[0];
  auto list = fp->argv[1];

  if (!Fixnum::IsType(nth))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "nth", nth);

  if (Fixnum::Int64Of(nth) < 0)
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "nth", nth);

  if (!Cons::IsList(list))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "nth", list);

  fp->value = Cons::Nth(list, Fixnum::Uint64Of(nth));
}

/** * (nthcdr fixnum list) => object **/
void Nthcdr(Frame* fp) {
  auto nth = fp->argv[0];
  auto list = fp->argv[1];

  if (!Cons::IsList(list))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "nthcdr",
                     list);

  if (!Fixnum::IsType(nth))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "nthcdr",
                     nth);

  if (Fixnum::Int64Of(nth) < 0)
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "nthcdr",
                     nth);

  fp->value = Cons::NthCdr(list, Fixnum::Uint64Of(nth));
}

} /* namespace mu */
} /* namespace libmu */
