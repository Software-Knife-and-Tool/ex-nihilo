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

/** * mu function (fixnum? fixnum) => bool **/
void IsFixnum(Env::Frame* fp) {
  fp->value = Type::GenBool(Fixnum::IsType(fp->argv[0]), fp->argv[0]);
}

/** * mu function (fixnum+ fixnum fixnum) => fixnum **/
void FixAdd(Env::Frame* fp) {
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

/** * mu function (fixnum- fixnum fixnum) => fixnum **/
void FixSub(Env::Frame* fp) {
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

/** * mu function (fixnum* fixnum fixnum) => fixnum **/
void FixMul(Env::Frame* fp) {
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

/** * mu function (fixnum< fixnum fixnum) => bool **/
void FixLessThan(Env::Frame* fp) {
  auto fx0 = fp->argv[0];
  auto fx1 = fp->argv[1];

  if (!Fixnum::IsType(fx0))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "fixnum<",
                     fx0);

  if (!Fixnum::IsType(fx1))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "fixnum<",
                     fx1);

  fp->value = Type::GenBool(Fixnum::Int64Of(fx0) < Fixnum::Int64Of(fx1), fx0);
}

/** * mu function (fixnum/ fixnum fixnum) => fixnum**/
void FixDiv(Env::Frame* fp) {
  auto fx0 = fp->argv[0];
  auto fx1 = fp->argv[1];

  if (!Fixnum::IsType(fx0))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "fixnum/",
                     fx0);

  if (!Fixnum::IsType(fx1))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "fixnum/",
                     fx1);

  if (Fixnum::Uint64Of(fx1) == 0)
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::DIVISION_BY_ZERO,
                     "fixnum/", fx1);

  fp->value = Fixnum(Fixnum::Int64Of(fx0) / Fixnum::Int64Of(fx1)).tag_;
}

/** * mu function (mod fixnum fixnum) => fixnum **/
void Mod(Env::Frame* fp) {
  auto fx0 = fp->argv[0];
  auto fx1 = fp->argv[1];

  if (!Fixnum::IsType(fx0))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "mod", fx0);

  if (!Fixnum::IsType(fx1))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "mod", fx1);

  fp->value = Fixnum(Fixnum::Uint64Of(fx0) % Fixnum::Uint64Of(fx1)).tag_;
}

/** * mu function (logand fixnum fixnum) => fixnum **/
void Logand(Env::Frame* fp) {
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

/** * mu function (logor fixnum fixnum) => fixnum **/
void Logor(Env::Frame* fp) {
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

/** * mu function (fixnum object) => fixnum **/
void FixnumCoerce(Env::Frame* fp) {
  auto fx = fp->argv[0];

  if (Char::IsType(fx))
    fp->value = Fixnum(Char::Uint8Of(fx)).tag_;
  else if (Float::IsType(fx))
    fp->value = Fixnum(static_cast<uint64_t>(Float::FloatOf(fx))).tag_;
  else if (Fixnum::IsType(fx))
    fp->value = fx;
  else
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "cannot be coerced to fixnum (fixnum)", fx);
}

} /* namespace mu */
} /* namespace libmu */
