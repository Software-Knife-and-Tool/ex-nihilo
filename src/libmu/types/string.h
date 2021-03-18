/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  string.h: char vectors
 **
 **/
#if !defined(LIBMU_TYPES_STRING_H_)
#define LIBMU_TYPES_STRING_H_

#include <cassert>
#include <cstring>
#include <functional>
#include <vector>

#include "libmu/type.h"

#include "libmu/types/char.h"
#include "libmu/types/fixnum.h"
#include "libmu/types/vector.h"

namespace libmu {
namespace core {

class Env;

/** * char vector **/
class String : public Vector {
 public: /* Tag */
  static constexpr bool IsType(Tag ptr) {
    return Vector::IsType(ptr) && Vector::TypeOf(ptr) == SYS_CLASS::CHAR;
  }

  static bool Eql(Tag str1, Tag str2) {
    assert(Vector::IsType(str2) && Vector::TypeOf(str2) == SYS_CLASS::CHAR);
    assert(Vector::IsType(str1) && Vector::TypeOf(str1) == SYS_CLASS::CHAR);

    return Eq(str1, str2) ||
           ((Vector::Length(str1) == Vector::Length(str2)) &&
            (std::memcmp(DataAddress<char>(str1), DataAddress<char>(str2),
                         Length(str1)) == 0));
  }

  static std::string StdStringOf(Tag str) {
    assert(IsType(str));

    return std::string(DataAddress<char>(str), Length(str));
  }

  static constexpr size_t Length(Tag str) {
    assert(IsType(str));

    return Vector::Length(str);
  }

  static Tag MakeImmediate(const std::string& str) {
    assert(str.size() <= IMMEDIATE_STR_MAX);

    uint64_t buffer = 0;

    std::memcpy(reinterpret_cast<char*>(&buffer), str.data(), str.size());

    return Type::MakeImmediate(buffer, str.size(), IMMEDIATE_CLASS::STRING);
  }

  static void Print(Env*, Tag, Tag, bool);
  static Tag Read(Env*, Tag);
  static Tag ViewOf(Env*, Tag);

 public: /* object model */
  explicit String(Env* env, std::string str) : Vector(env, str) {}
};

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TYPES_STRING_H_ */
