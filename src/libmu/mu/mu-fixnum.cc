/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu-fixnum.cc: library fixnums
 **
 **/
#include <cassert>

#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/types/exception.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/float.h"

namespace libmu {
namespace mu {

using Exception = core::Exception;
using Fixnum = core::Fixnum;
using Frame = core::Env::Frame;
using Type = core::Type;

/** * (fixnum? fixnum) => bool **/
void IsFixnum(Frame* fp) {
  fp->value = Type::Bool(Fixnum::IsType(fp->argv[0]));
}

/** * (fixnum+ fixnum fixnum) => fixnum **/
void FixAdd(Frame* fp) {
  auto fx0 = fp->argv[0];
  auto fx1 = fp->argv[1];

  if (!Fixnum::IsType(fx0))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "fixnum+",
                     fx0);

  if (!Fixnum::IsType(fx1))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "fixnum+",
                     fx1);

  fp->value = Fixnum(Fixnum::Int64Of(fx0) + Fixnum::Int64Of(fx1)).tag_;
}

/** * (fixnum- fixnum fixnum) => fixnum **/
void FixSub(Frame* fp) {
  auto fx0 = fp->argv[0];
  auto fx1 = fp->argv[1];

  if (!Fixnum::IsType(fx0))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "fixnum-",
                     fx0);
  if (!Fixnum::IsType(fx1))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "fixnum-",
                     fx1);

  fp->value = Fixnum(Fixnum::Int64Of(fx0) - Fixnum::Int64Of(fx1)).tag_;
}

/** * (fixnum* fixnum fixnum) => fixnum **/
void FixMul(Frame* fp) {
  auto fx0 = fp->argv[0];
  auto fx1 = fp->argv[1];

  if (!Fixnum::IsType(fx0))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "fixnum*",
                     fx0);
  if (!Fixnum::IsType(fx1))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "fixnum*",
                     fx1);

  fp->value = Fixnum(Fixnum::Int64Of(fx0) * Fixnum::Int64Of(fx1)).tag_;
}

/** * (fixnum< fixnum fixnum) => bool **/
void FixLessThan(Frame* fp) {
  auto fx0 = fp->argv[0];
  auto fx1 = fp->argv[1];

  if (!Fixnum::IsType(fx0))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "fixnum<",
                     fx0);

  if (!Fixnum::IsType(fx1))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "fixnum<",
                     fx1);

  fp->value = Type::Bool(Fixnum::Int64Of(fx0) < Fixnum::Int64Of(fx1));
}

/** * (truncate fixnum fixnum) => (fixnum . fixnum) **/
void Truncate(Frame* fp) {
  auto fx0 = fp->argv[0];
  auto fx1 = fp->argv[1];

  if (!Fixnum::IsType(fx0))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "truncate",
                     fx0);

  if (!Fixnum::IsType(fx1))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "truncate",
                     fx1);

  if (Fixnum::Uint64Of(fx1) == 0)
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::DIVISION_BY_ZERO,
                     "truncate", fx1);

  auto ifx0 = Fixnum::Int64Of(fx0);
  auto ifx1 = Fixnum::Int64Of(fx1);
  auto quot = ifx0 < ifx1 ? 0 : ifx0 / ifx1;
  auto rem = quot == 0 ? ifx0 : ifx0 - (ifx1 * quot);

  fp->value = core::Cons(Fixnum(quot).tag_, Fixnum(rem).tag_).Evict(fp->env);
}

/** * (floor fixnum fixnum) => (fixnum . fixnum) **/
void Floor(Frame* fp) {
  auto number = fp->argv[0];
  auto divisor = fp->argv[1];

  /* quotient * divisor + remainder = number */

  if (!Fixnum::IsType(number))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "floor",
                     number);

  if (!Fixnum::IsType(divisor))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "floor",
                     divisor);

  if (Fixnum::Uint64Of(divisor) == 0)
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::DIVISION_BY_ZERO,
                     "floor", divisor);

  auto nx = Fixnum::Int64Of(number);
  auto dx = Fixnum::Int64Of(divisor);

  auto rem = nx - (nx / dx) * dx;
  auto quot = (nx - rem) / dx;

  fp->value = core::Cons(Fixnum(quot).tag_, Fixnum(rem).tag_).Evict(fp->env);
}

/** * (logand fixnum fixnum) => fixnum **/
void Logand(Frame* fp) {
  auto fx0 = fp->argv[0];
  auto fx1 = fp->argv[1];

  if (!Fixnum::IsType(fx0))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "logand",
                     fx0);

  if (!Fixnum::IsType(fx1))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "logand",
                     fx1);

  fp->value = Fixnum(Fixnum::Uint64Of(fx0) & Fixnum::Uint64Of(fx1)).tag_;
}

/** * (logor fixnum fixnum) => fixnum **/
void Logor(Frame* fp) {
  auto fx0 = fp->argv[0];
  auto fx1 = fp->argv[1];

  if (!Fixnum::IsType(fx0))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "logor",
                     fx0);

  if (!Fixnum::IsType(fx1))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "logor",
                     fx1);

  fp->value = Fixnum(Fixnum::Uint64Of(fx0) | Fixnum::Uint64Of(fx1)).tag_;
}

} /* namespace mu */
} /* namespace libmu */
