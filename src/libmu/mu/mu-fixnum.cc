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

using Frame = Env::Frame;

/** * (fixnum? fixnum) => bool **/
void IsFixnum(Frame* fp) {
  fp->value = Type::GenBool(Fixnum::IsType(fp->argv[0]), fp->argv[0]);
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

  fp->value = Type::GenBool(Fixnum::Int64Of(fx0) < Fixnum::Int64Of(fx1), fx0);
}

/** * (ceiling fixnum fixnum) => (fixnum . fixnum) **/
void Ceiling(Frame* fp) {
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

/** * (floor fixnum fixnum) => (fixnum . fixnum) **/
void Floor(Frame* fp) {
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

/** * (round fixnum fixnum) => (fixnum . fixnum) **/
void Round(Frame* fp) {
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

/** * (truncate fixnum fixnum) => (fixnum . fixnum) **/
void Truncate(Frame* fp) {
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

/** * (mod fixnum fixnum) => fixnum **/
void Mod(Frame* fp) {
  auto fx0 = fp->argv[0];
  auto fx1 = fp->argv[1];

  if (!Fixnum::IsType(fx0))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "mod", fx0);

  if (!Fixnum::IsType(fx1))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "mod", fx1);

  fp->value = Fixnum(Fixnum::Uint64Of(fx0) % Fixnum::Uint64Of(fx1)).tag_;
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
