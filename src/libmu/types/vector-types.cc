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
    vlist.push_back(Function::Funcall(env, func, std::vector<TagPtr>{T(*it).tag_}));
  return Vector(env, vlist).tag_;
}

template <typename T, typename S>
static void VMapC(Env* env, TagPtr func, TagPtr vector) {
  assert(Function::IsType(func));
  assert(Vector::IsType(vector));

  std::vector<S> vlist;
  Vector::vector_iter<S> iter(vector);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    (void)Function::Funcall(env, func, std::vector<TagPtr>{T(*it).tag_});
}

} /* anynoymous namespace */

/** * map a function onto a vector **/
TagPtr Vector::Map(Env* env, TagPtr func, TagPtr vector) {
  assert(Function::IsType(func));
  assert(Vector::IsType(vector));

  switch (Type::MapSymbolClass(Vector::VecType(vector))) {
    case SYS_CLASS::T: {
      return VMap<Vector, TagPtr>(env, func, vector);
    }
    case SYS_CLASS::FLOAT: {
      // return VMap<Float, float>(env, func, vector);
      std::vector<float> vlist;
      Vector::vector_iter<float> iter(vector);
      for (auto it = iter.begin(); it != iter.end(); it = ++iter)
        vlist.push_back(Float::FloatOf(Function::Funcall(
            env, func, std::vector<TagPtr>{Float(*it).tag_})));
      return Vector(env, vlist).tag_;
    }
    case SYS_CLASS::FIXNUM: {
      std::vector<int64_t> vlist;
      Vector::vector_iter<int64_t> iter(vector);
      for (auto it = iter.begin(); it != iter.end(); it = ++iter)
        vlist.push_back(Fixnum::Int64Of(Function::Funcall(
            env, func, std::vector<TagPtr>{Fixnum(*it).tag_})));
      return Vector(env, vlist).tag_;
    }
    case SYS_CLASS::CHAR: {
      std::string vlist;
      Vector::vector_iter<char> iter(vector);
      for (auto it = iter.begin(); it != iter.end(); it = ++iter)
        vlist.push_back(static_cast<char>(Char::Uint8Of(Function::Funcall(
            env, func, std::vector<TagPtr>{Char(*it).tag_}))));
      return Vector(env, vlist).tag_;
    }
    case SYS_CLASS::BYTE: {
      std::vector<uint8_t> vlist;
      Vector::vector_iter<uint8_t> iter(vector);
      for (auto it = iter.begin(); it != iter.end(); it = ++iter)
        vlist.push_back(static_cast<uint8_t>(Fixnum::Uint64Of(Function::Funcall(
            env, func, std::vector<TagPtr>{Fixnum(*it).tag_}))));
      return Vector(env, vlist).tag_;
    }
    default:
      assert(!"vector type botch");
  }
}

/** * map a function over a vector **/
void Vector::MapC(Env* env, TagPtr func, TagPtr vector) {
  assert(Function::IsType(func));
  assert(Vector::IsType(vector));

  switch (Type::MapSymbolClass(Vector::VecType(vector))) {
    case SYS_CLASS::T: {
      return VMapC<Vector, TagPtr>(env, func, vector);
      /*
      Vector::vector_iter<TagPtr> iter(vector);
      for (auto it = iter.begin(); it != iter.end(); it = ++iter)
        (void)Function::Funcall(env, func, std::vector<TagPtr>{*it});
      */
      break;
    }
    case SYS_CLASS::FLOAT: {
      Vector::vector_iter<float> iter(vector);
      for (auto it = iter.begin(); it != iter.end(); it = ++iter)
        (void)Function::Funcall(env, func,
                                std::vector<TagPtr>{Float(*it).tag_});
      break;
    }
    case SYS_CLASS::FIXNUM: {
      Vector::vector_iter<int64_t> iter(vector);
      for (auto it = iter.begin(); it != iter.end(); it = ++iter)
        (void)Function::Funcall(env, func,
                                std::vector<TagPtr>{Fixnum(*it).tag_});
      break;
    }
    case SYS_CLASS::CHAR: {
      Vector::vector_iter<char> iter(vector);
      for (auto it = iter.begin(); it != iter.end(); it = ++iter)
        (void)Function::Funcall(env, func, std::vector<TagPtr>{Char(*it).tag_});
      break;
    }
    case SYS_CLASS::BYTE: {
      Vector::vector_iter<uint8_t> iter(vector);
      for (auto it = iter.begin(); it != iter.end(); it = ++iter)
        (void)Function::Funcall(env, func,
                                std::vector<TagPtr>{Fixnum(*it).tag_});
      break;
    }
    default:
      assert(!"vector type botch");
  }
}

/** * list to vector **/
TagPtr Vector::ListToVector(Env* env, TagPtr vectype, TagPtr list) {
  assert(Cons::IsList(list));

  auto vtype = Type::MapSymbolClass(vectype);

  /** * #(:type) returns empty typed vector */
  if (Null(list)) switch (vtype) {
      case SYS_CLASS::T:
        return Vector(env, std::vector<TagPtr>()).tag_;
      case SYS_CLASS::FLOAT:
        return Vector(env, std::vector<float>()).tag_;
      case SYS_CLASS::FIXNUM:
        return Vector(env, std::vector<int64_t>()).tag_;
      case SYS_CLASS::BYTE:
        return Vector(env, std::vector<uint8_t>()).tag_;
      case SYS_CLASS::CHAR:
        return Vector(env, std::string()).tag_;
      default:
        assert(!"vector type botch");
    }

  std::vector<TagPtr> vlistT;
  std::vector<float> vlistFloat;
  std::vector<int64_t> vlistFixnum;
  std::vector<uint8_t> vlistByte;
  std::string vlistChar;

  Cons::cons_iter<TagPtr> iter(list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
    auto form = it->car;

    switch (vtype) {
      case SYS_CLASS::T:
        vlistT.push_back(form);
        break;
      case SYS_CLASS::FLOAT:
        if (!Float::IsType(form))
          Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                           "type mismatch in vector initialization", form);
        vlistFloat.push_back(Float::FloatOf(form));
        break;
      case SYS_CLASS::FIXNUM:
        if (!Fixnum::IsType(form))
          Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                           "type mismatch in vector initialization", form);
        vlistFixnum.push_back(Fixnum::Int64Of(form));
        break;
      case SYS_CLASS::CHAR:
        if (!Char::IsType(form))
          Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                           "type mismatch in vector initialization", form);
        vlistChar.push_back(static_cast<char>(Char::Uint8Of(form)));
        break;
      case SYS_CLASS::BYTE:
        if (!Fixnum::IsByte(form))
          Exception::Raise(env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                           "type mismatch in vector initialization", form);
        vlistByte.push_back(static_cast<uint8_t>(Fixnum::Uint64Of(form)));
        break;
      default:
        assert(!"vector type botch");
    }
  }

  switch (vtype) {
    case SYS_CLASS::T:
      return Vector(env, vlistT).tag_;
    case SYS_CLASS::FLOAT:
      return Vector(env, vlistFloat).tag_;
    case SYS_CLASS::FIXNUM:
      return Vector(env, vlistFixnum).tag_;
    case SYS_CLASS::BYTE:
      return Vector(env, vlistByte).tag_;
    case SYS_CLASS::CHAR:
      return Vector(env, vlistChar).tag_;
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
