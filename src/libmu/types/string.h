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
  static constexpr auto IsType(Tag ptr) -> bool {
    return Vector::IsType(ptr) && Vector::TypeOf(ptr) == Type::SYS_CLASS::CHAR;
  }

  static auto Eql(Tag str1, Tag str2) -> bool {
    assert(Vector::IsType(str2) &&
           Vector::TypeOf(str2) == Type::SYS_CLASS::CHAR);
    assert(Vector::IsType(str1) &&
           Vector::TypeOf(str1) == Type::SYS_CLASS::CHAR);

    return Type::Eq(str1, str2) ||
           ((Vector::Length(str1) == Vector::Length(str2)) &&
            (std::memcmp(Data<char>(str1), Data<char>(str2), Length(str1)) ==
             0));
  }

  static auto StdStringOf(Tag str) -> std::string {
    assert(IsType(str));

    return std::string(Data<char>(str), Length(str));
  }

  static constexpr auto Length(Tag str) -> size_t {
    assert(IsType(str));

    return Vector::Length(str);
  }

  static auto MakeImmediate(const std::string& str) -> Tag {
    assert(str.size() <= IMMEDIATE_STR_MAX);

    uint64_t buffer = 0;

    std::memcpy(reinterpret_cast<char*>(&buffer), str.data(), str.size());

    return Type::MakeImmediate(buffer, str.size(), IMMEDIATE_CLASS::STRING);
  }

  static auto Print(Env*, Tag, Tag, bool) -> void;
  static auto Read(Env*, Tag) -> Tag;
  static auto ViewOf(Env* env, Tag) -> Tag;

#if 0
    if (src.size() <= IMMEDIATE_STR_MAX) {
    tag_ = String::MakeImmediate(std::string(src.begin(), src.end()));
#endif
 public: /* object */
  explicit String(Env* env, std::string src)
    : VectorT<SYS_CLASS::STRING, char>(env, std::vector<char>(src.begin(), src.end())) {}
};

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TYPES_STRING_H_ */
