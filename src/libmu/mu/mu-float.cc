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

using Exception = core::Exception;
using Float = core::Float;
using Frame = core::Env::Frame;
using Type = core::Type;

/** * (float? object) => bool **/
void IsFloat(Frame* fp) { fp->value = Type::Bool(Float::IsType(fp->argv[0])); }

/** * (float+ float float) => float **/
void FloatAdd(Frame* fp) {
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
void FloatSub(Frame* fp) {
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
void FloatMul(Frame* fp) {
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
void FloatDiv(Frame* fp) {
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
void FloatLessThan(Frame* fp) {
  auto fl0 = fp->argv[0];
  auto fl1 = fp->argv[1];

  if (!Float::IsType(fl0))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "float<",
                     fl0);

  if (!Float::IsType(fl1))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "float<",
                     fl1);

  fp->value = Type::Bool(Float::FloatOf(fl0) < Float::FloatOf(fl1));
}

/** * (sqrt float) => float **/
void Sqrt(Frame* fp) {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(sqrt)",
                     scalar);

  fp->value = Float(sqrt(Float::FloatOf(scalar))).tag_;
}

/** * (pow float float') => float **/
void Pow(Frame* fp) {
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
void Atan(Frame* fp) {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(exp)",
                     scalar);

  fp->value = Float(atan(Float::FloatOf(scalar))).tag_;
}

/** * (asin float) => float **/
void Asin(Frame* fp) {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(exp)",
                     scalar);

  fp->value = Float(asin(Float::FloatOf(scalar))).tag_;
}

/** * (acos float) => float **/
void Acos(Frame* fp) {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(exp)",
                     scalar);

  fp->value = Float(acos(Float::FloatOf(scalar))).tag_;
}

/** * (exp float) => float **/
void Exp(Frame* fp) {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(exp)",
                     scalar);

  fp->value = Float(exp(Float::FloatOf(scalar))).Evict(fp->env, "mu-float:exp");
}

/** * (log float) => float **/
void Log(Frame* fp) {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(log)",
                     scalar);

  fp->value = Float(log(Float::FloatOf(scalar))).tag_;
}

/** * (log10 float) => float **/
void Log10(Frame* fp) {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(log)",
                     scalar);

  fp->value = Float(log10(Float::FloatOf(scalar))).tag_;
}

/** * (sin float) => float **/
void Sine(Frame* fp) {
  auto theta = fp->argv[0];

  if (!Float::IsType(theta))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(sin)",
                     theta);

  fp->value = Float(sin(Float::FloatOf(theta) * 3.14159265 / 180.0)).tag_;
}

/** * (cos float) => float **/
void Cosine(Frame* fp) {
  auto theta = fp->argv[0];

  if (!Float::IsType(theta))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(cos)",
                     theta);

  fp->value = Float(cos(Float::FloatOf(theta) * 3.14159265 / 180.0)).tag_;
}

/** * (tan float) => float **/
void Tangent(Frame* fp) {
  auto theta = fp->argv[0];

  if (!Float::IsType(theta))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(tan)",
                     theta);

  fp->value = Float(tan(Float::FloatOf(theta) * 3.14159265 / 180.0)).tag_;
}

} /* namespace mu */
} /* namespace libmu */
