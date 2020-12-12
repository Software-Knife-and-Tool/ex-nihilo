/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  vector-types.cc: library specialized vectors
 **
 **/
#include "libmu/types/vector.h"

#include <cassert>

#include "libmu/env.h"
#include "libmu/print.h"
#include "libmu/read.h"
#include "libmu/type.h"

#include "libmu/types/char.h"
#include "libmu/types/cons.h"
#include "libmu/types/exception.h"
#include "libmu/types/float.h"
#include "libmu/types/function.h"
#include "libmu/types/stream.h"

namespace libmu {
namespace {

template <typename T, typename S>
static auto VMap(Env* env, TagPtr func, TagPtr vector) {
  assert(Function::IsType(func));
  assert(Vector::IsType(vector));

  std::vector<S> vlist;
  Vector::vector_iter<S> iter(vector);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    vlist.push_back(T::VSpecOf(
        Function::Funcall(env, func, std::vector<TagPtr>{T(*it).tag_})));
  return Vector(env, vlist).tag_;
}

template <typename T, typename S>
static void VMapC(Env* env, TagPtr func, TagPtr vector) {
  assert(Function::IsType(func));
  assert(Vector::IsType(vector));

  Vector::vector_iter<S> iter(vector);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    (void)Function::Funcall(env, func, std::vector<TagPtr>{T(*it).tag_});
}

template <typename T, typename S>
static TagPtr VList(Env* env, std::function<bool(TagPtr)> isType, TagPtr list) {
  std::vector<S> vec;

  Cons::cons_iter<TagPtr> iter(list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
    auto form = it->car;
    if (!isType(form))
      Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                       "type mismatch in vector initialization", form);
    vec.push_back(T::VSpecOf(form));
  }
  return Vector(env, vec).tag_;
}

} /* anonymous namespace */

/** * map a function onto a vector **/
TagPtr Vector::Map(Env* env, TagPtr func, TagPtr vector) {
  assert(Function::IsType(func));
  assert(Vector::IsType(vector));

  switch (Type::MapSymbolClass(Vector::VecType(vector))) {
    case SYS_CLASS::T:
      return VMap<Vector, TagPtr>(env, func, vector);
    case SYS_CLASS::FLOAT:
      return VMap<Float, float>(env, func, vector);
    case SYS_CLASS::FIXNUM:
      return VMap<Fixnum, int64_t>(env, func, vector);
    case SYS_CLASS::CHAR:
      return VMap<Char, char>(env, func, vector);
    case SYS_CLASS::BYTE:
      return VMap<Fixnum, uint8_t>(env, func, vector);
    default:
      assert(!"vector type botch");
  }
}

/** * map a function over a vector **/
void Vector::MapC(Env* env, TagPtr func, TagPtr vector) {
  assert(Function::IsType(func));
  assert(Vector::IsType(vector));

  switch (Type::MapSymbolClass(Vector::VecType(vector))) {
    case SYS_CLASS::T:
      return VMapC<Vector, TagPtr>(env, func, vector);
    case SYS_CLASS::FLOAT:
      return VMapC<Float, float>(env, func, vector);
    case SYS_CLASS::FIXNUM:
      return VMapC<Fixnum, uint64_t>(env, func, vector);
    case SYS_CLASS::CHAR:
      return VMapC<Char, char>(env, func, vector);
    case SYS_CLASS::BYTE:
      return VMapC<Fixnum, uint8_t>(env, func, vector);
    default:
      assert(!"vector type botch");
  }
}

/** * list to vector **/
TagPtr Vector::ListToVector(Env* env, TagPtr vectype, TagPtr list) {
  assert(Cons::IsList(list));

  auto vtype = Type::MapSymbolClass(vectype);

  switch (vtype) {
    case SYS_CLASS::T:
      return VList<Vector, TagPtr>(
          env, [](TagPtr) { return true; }, list);
    case SYS_CLASS::FLOAT:
      return VList<Float, float>(env, Float::IsType, list);
    case SYS_CLASS::FIXNUM:
      return VList<Fixnum, int64_t>(env, Fixnum::IsType, list);
    case SYS_CLASS::BYTE:
      return VList<Fixnum, uint8_t>(env, Fixnum::IsType, list);
    case SYS_CLASS::CHAR:
      return VList<Char, char>(env, Char::IsType, list);
    default:
      assert(!"vector type botch");
  }
}

/** * print vector to stream **/
void Vector::Print(Env* env, TagPtr vector, TagPtr stream, bool esc) {
  assert(IsType(vector));
  assert(Stream::IsType(stream));

  switch (Vector::TypeOf(vector)) {
    case SYS_CLASS::BYTE: {
      core::PrintStdString(env, "#(:byte", stream, false);

      vector_iter<uint8_t> iter(vector);
      for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
        core::PrintStdString(env, " ", stream, false);
        core::Print(env, Fixnum(*iter).tag_, stream, esc);
      }

      core::PrintStdString(env, ")", stream, false);
      break;
    }
    case SYS_CLASS::CHAR: {
      if (esc) core::PrintStdString(env, "\"", stream, false);

      vector_iter<char> iter(vector);
      for (auto it = iter.begin(); it != iter.end(); it = ++iter)
        core::Print(env, Char(*it).tag_, stream, false);

      if (esc) core::PrintStdString(env, "\"", stream, false);
      break;
    }
    case SYS_CLASS::FIXNUM: {
      core::PrintStdString(env, "#(:fixnum", stream, false);

      vector_iter<int64_t> iter(vector);
      for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
        core::PrintStdString(env, " ", stream, false);
        core::Print(env, Fixnum(*iter).tag_, stream, esc);
      }

      core::PrintStdString(env, ")", stream, false);
      break;
    }
    case SYS_CLASS::FLOAT: {
      core::PrintStdString(env, "#(:float", stream, false);

      vector_iter<float> iter(vector);
      for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
        core::PrintStdString(env, " ", stream, false);
        core::Print(env, Float(*iter).tag_, stream, esc);
      }

      core::PrintStdString(env, ")", stream, false);
      break;
    }
    case SYS_CLASS::T: {
      core::PrintStdString(env, "#(:t", stream, false);

      vector_iter<TagPtr> iter(vector);
      for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
        core::PrintStdString(env, " ", stream, false);
        core::Print(env, *iter, stream, esc);
      }

      core::PrintStdString(env, ")", stream, false);
      break;
    }
    default:
      assert(!"vector type botch");
      break;
  }
}

} /* namespace libmu */
