/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu-float.cc: library float functions
 **
 **/
#include <cmath>

#include <cassert>
#include <limits>

#include "libmu/env.h"

#include "libmu/types/condition.h"
#include "libmu/types/float.h"

namespace libmu {
namespace mu {

using Condition = core::Condition;
using Float = core::Float;
using Frame = core::Env::Frame;
using Type = core::Type;

/** * (float? object) => bool **/
auto IsFloat(Frame* fp) -> void {
  fp->value = Type::Bool(Float::IsType(fp->argv[0]));
}

/** * (float+ float float) => float **/
auto FloatAdd(Frame* fp) -> void {
  auto fl0 = fp->argv[0];
  auto fl1 = fp->argv[1];

  if (!Float::IsType(fl0))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "float+",
                     fl0);
  if (!Float::IsType(fl1))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "float+",
                     fl1);

  fp->value = Float(Float::FloatOf(fl0) + Float::FloatOf(fl1)).tag_;
}

/** * (float- float float) => float **/
auto FloatSub(Frame* fp) -> void {
  auto fl0 = fp->argv[0];
  auto fl1 = fp->argv[1];

  if (!Float::IsType(fl0))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "float-",
                     fl0);

  if (!Float::IsType(fl1))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "float-",
                     fl1);

  fp->value = Float(Float::FloatOf(fl0) - Float::FloatOf(fl1)).tag_;
}

/** * (float* float float) => float**/
auto FloatMul(Frame* fp) -> void {
  auto fl0 = fp->argv[0];
  auto fl1 = fp->argv[1];

  if (!Float::IsType(fl0))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "float*",
                     fl0);

  if (!Float::IsType(fl1))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "float*",
                     fl1);

  fp->value = Float(Float::FloatOf(fl0) * Float::FloatOf(fl1)).tag_;
}

/** * (float/ float float) => float **/
auto FloatDiv(Frame* fp) -> void {
  auto fl0 = fp->argv[0];
  auto fl1 = fp->argv[1];

  if (!Float::IsType(fl0))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "float/",
                     fl0);

  if (!Float::IsType(fl1))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "float/",
                     fl1);

  fp->value = Float(Float::FloatOf(fl0) / Float::FloatOf(fl1)).tag_;
}

/** * (float< float float) => bool **/
auto FloatLessThan(Frame* fp) -> void {
  auto fl0 = fp->argv[0];
  auto fl1 = fp->argv[1];

  if (!Float::IsType(fl0))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "float<",
                     fl0);

  if (!Float::IsType(fl1))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "float<",
                     fl1);

  fp->value = Type::Bool(Float::FloatOf(fl0) < Float::FloatOf(fl1));
}

/** * (sqrt float) => float **/
auto Sqrt(Frame* fp) -> void {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "(sqrt)",
                     scalar);

  fp->value = Float(sqrtf(Float::FloatOf(scalar))).tag_;
}

/** * (pow float float') => float **/
auto Pow(Frame* fp) -> void {
  auto base = fp->argv[0];
  auto exp = fp->argv[1];

  if (!Float::IsType(base))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "(pow)",
                     base);

  if (!Float::IsType(exp))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "(pow)",
                     exp);

  fp->value = Float(powf(Float::FloatOf(base), Float::FloatOf(exp))).tag_;
}

/** * (atan float) => float **/
auto Atan(Frame* fp) -> void {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "(exp)",
                     scalar);

  fp->value = Float(atanf(Float::FloatOf(scalar))).tag_;
}

/** * (asin float) => float **/
auto Asin(Frame* fp) -> void {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "(exp)",
                     scalar);

  fp->value = Float(asinf(Float::FloatOf(scalar))).tag_;
}

/** * (acos float) => float **/
auto Acos(Frame* fp) -> void {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "(exp)",
                     scalar);

  fp->value = Float(acosf(Float::FloatOf(scalar))).tag_;
}

/** * (exp float) => float **/
auto Exp(Frame* fp) -> void {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "(exp)",
                     scalar);

  fp->value = Float(expf(Float::FloatOf(scalar))).Evict(fp->env);
}

/** * (log float) => float **/
auto Log(Frame* fp) -> void {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "(log)",
                     scalar);

  fp->value = Float(logf(Float::FloatOf(scalar))).tag_;
}

/** * (log10 float) => float **/
auto Log10(Frame* fp) -> void {
  auto scalar = fp->argv[0];

  if (!Float::IsType(scalar))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "(log)",
                     scalar);

  fp->value = Float(log10f(Float::FloatOf(scalar))).tag_;
}

/** * (sin float) => float **/
auto Sine(Frame* fp) -> void {
  auto theta = fp->argv[0];

  if (!Float::IsType(theta))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "(sin)",
                     theta);

  fp->value = Float(sinf(Float::FloatOf(theta) * 3.14159265 / 180.0)).tag_;
}

/** * (cos float) => float **/
auto Cosine(Frame* fp) -> void {
  auto theta = fp->argv[0];

  if (!Float::IsType(theta))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "(cos)",
                     theta);

  fp->value = Float(cosf(Float::FloatOf(theta) * 3.14159265 / 180.0)).tag_;
}

/** * (tan float) => float **/
auto Tangent(Frame* fp) -> void {
  auto theta = fp->argv[0];

  if (!Float::IsType(theta))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR, "(tan)",
                     theta);

  fp->value = Float(tanf(Float::FloatOf(theta) * 3.14159265 / 180.0)).tag_;
}

} /* namespace mu */
} /* namespace libmu */
