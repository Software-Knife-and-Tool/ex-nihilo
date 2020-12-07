/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu-float.cc: library float functions
 **
 **/
#include <math.h>

#include <cassert>
#include <limits>

#include "libmu/env.h"

#include "libmu/types/exception.h"
#include "libmu/types/float.h"

namespace libmu {
namespace mu {

/** * (float? object) => bool **/
void IsFloat(Env::Frame* fp) {
  fp->value = Type::GenBool(Float::IsType(fp->argv[0]), fp->argv[0]);
}

/** * (float object) => float **/
void FloatCoerce(Env::Frame* fp) {
  auto fl = fp->argv[0];

  fp->value = Type::NIL;

  if (Fixnum::IsType(fl))
    fp->value = Float(static_cast<float>(Fixnum::Int64Of(fl))).tag_;
  else if (Float::IsType(fl))
    fp->value = fl;
  else
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "cannot be converted to float (float)", fl);
}

/** * (float+ float float) => float **/
void FloatAdd(Env::Frame* fp) {
  auto fl0 = fp->argv[0];
  auto fl1 = fp->argv[1];

  if (!Float::IsType(fl0))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "float+",
                     fl0);
  if (!Float::IsType(fl1))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "float+",
                     fl1);

  fp->value = Float(Float::FloatOf(fl0) + Float::FloatOf(fl1)).tag_;
}

/** * (float- float float) => float **/
void FloatSub(Env::Frame* fp) {
  auto fl0 = fp->argv[0];
  auto fl1 = fp->argv[1];

  if (!Float::IsType(fl0))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "float-",
                     fl0);

  if (!Float::IsType(fl1))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "float-",
                     fl1);

  fp->value = Float(Float::FloatOf(fl0) - Float::FloatOf(fl1)).tag_;
}

/** * (float* float float) => float**/
void FloatMul(Env::Frame* fp) {
  auto fl0 = fp->argv[0];
  auto fl1 = fp->argv[1];

  if (!Float::IsType(fl0))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "float*",
                     fl0);

  if (!Float::IsType(fl1))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "float*",
                     fl1);

  fp->value = Float(Float::FloatOf(fl0) * Float::FloatOf(fl1)).tag_;
}

/** * (float/ float float) => float **/
void FloatDiv(Env::Frame* fp) {
  auto fl0 = fp->argv[0];
  auto fl1 = fp->argv[1];

  if (!Float::IsType(fl0))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "float/",
                     fl0);

  if (!Float::IsType(fl1))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "float/",
                     fl1);

  fp->value = Float(Float::FloatOf(fl0) / Float::FloatOf(fl1)).tag_;
}

/** * (float< float float) => bool **/
void FloatLessThan(Env::Frame* fp) {
  auto fl0 = fp->argv[0];
  auto fl1 = fp->argv[1];

  if (!Float::IsType(fl0))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "float<",
                     fl0);

  if (!Float::IsType(fl1))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "float<",
                     fl1);

  fp->value = Type::GenBool(Float::FloatOf(fl0) < Float::FloatOf(fl1), fl0);
}

/** * (sqrt float) => float **/
void Sqrt(Env::Frame* fp) {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(sqrt)",
                     scalar);

  fp->value = Float(sqrt(Float::FloatOf(scalar))).tag_;
}

/** * (pow float float') => float **/
void Pow(Env::Frame* fp) {
  auto base = fp->argv[0];
  auto exp = fp->argv[1];

  if (!Float::IsType(base))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(pow)",
                     base);

  if (!Float::IsType(exp))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(pow)",
                     exp);

  fp->value = Float(pow(Float::FloatOf(base), Float::FloatOf(exp))).tag_;
}

/** * (atan float) => float **/
void Atan(Env::Frame* fp) {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(exp)",
                     scalar);

  fp->value = Float(atan(Float::FloatOf(scalar))).tag_;
}

/** * (asin float) => float **/
void Asin(Env::Frame* fp) {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(exp)",
                     scalar);

  fp->value = Float(asin(Float::FloatOf(scalar))).tag_;
}

/** * (acos float) => float **/
void Acos(Env::Frame* fp) {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(exp)",
                     scalar);

  fp->value = Float(acos(Float::FloatOf(scalar))).tag_;
}

/** * (exp float) => float **/
void Exp(Env::Frame* fp) {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(exp)",
                     scalar);

  fp->value = Float(exp(Float::FloatOf(scalar))).Evict(fp->env, "mu-float:exp");
}

/** * (log float) => float **/
void Log(Env::Frame* fp) {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(log)",
                     scalar);

  fp->value = Float(log(Float::FloatOf(scalar))).tag_;
}

/** * (log10 float) => float **/
void Log10(Env::Frame* fp) {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(log)",
                     scalar);

  fp->value = Float(log10(Float::FloatOf(scalar))).tag_;
}

/** * (sin float) => float **/
void Sine(Env::Frame* fp) {
  auto theta = fp->argv[0];

  if (!Float::IsType(theta))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(sin)",
                     theta);

  fp->value = Float(sin(Float::FloatOf(theta) * 3.14159265 / 180.0)).tag_;
}

/** * (cos float) => float **/
void Cosine(Env::Frame* fp) {
  auto theta = fp->argv[0];

  if (!Float::IsType(theta))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(cos)",
                     theta);

  fp->value = Float(cos(Float::FloatOf(theta) * 3.14159265 / 180.0)).tag_;
}

/** * (tan float) => float **/
void Tangent(Env::Frame* fp) {
  auto theta = fp->argv[0];

  if (!Float::IsType(theta))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(tan)",
                     theta);

  fp->value = Float(tan(Float::FloatOf(theta) * 3.14159265 / 180.0)).tag_;
}

} /* namespace mu */
} /* namespace libmu */
