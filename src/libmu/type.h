/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  type.h: library tagged pointers
 **
 **/
#if !defined(_LIBMU_TYPE_H_)
#define _LIBMU_TYPE_H_

#include <cassert>
#include <cinttypes>
#include <functional>
#include <map>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace libmu {

class Env;

/** * abstract base class for types **/
class Type {
 public:
  enum class TagPtr : uint64_t {};

  template <typename E>
  static constexpr auto to_underlying(E e) noexcept {
    return static_cast<std::underlying_type_t<E>>(e);
  }

 public: /* TagPtr layout */
  /** * 3 bit staged low tag **/
  enum class TAG : uint8_t {
    ADDRESS = 0,   /* machine address */
    EFIXNUM = 1,   /* even fixnum (62 bits) */
    SYMBOL = 2,    /* symbol/keyword */
    FUNCTION = 3,  /* function */
    CONS = 4,      /* cons */
    OFIXNUM = 5,   /* odd fixnum (62 bits) */
    IMMEDIATE = 6, /* immediate */
    EXTEND = 7     /* extended */
  };

  /** * extract tag **/
  static constexpr TAG TagOf(TagPtr ptr) {
    return static_cast<TAG>(to_underlying(ptr) & 0x7);
  }

  /** * make tagged pointer **/
  static constexpr TagPtr Entag(TagPtr ptr, TAG tag) {
    return static_cast<TagPtr>(to_underlying(ptr) | to_underlying(tag));
  }

  /** * address to tagged pointer **/
  static TagPtr Entag(void* caddr, TAG tag) {
    return static_cast<TagPtr>(reinterpret_cast<uint64_t>(caddr) |
                               to_underlying(tag));
  }

  /** * tagged pointer to type pointer **/
  template <typename T>
  static T* Untag(TagPtr ptr) {
    return reinterpret_cast<T*>(to_underlying(ptr) & ~0x7);
  }

  /** * as an address **/
  static void* ToAddress(TagPtr ptr) {
    return reinterpret_cast<void*>(to_underlying(ptr));
  }

  /** * from uint64_t **/
  static TagPtr FromUint64(uint64_t ptr) { return static_cast<TagPtr>(ptr); }

  /** * to uint64_t **/
  static constexpr uint64_t ToUint64(TagPtr ptr) { return to_underlying(ptr); }

  /** * extended immediate **/
  enum class IMMEDIATE_CLASS : uint8_t {
    CHAR = 0,
    STRING = 1,
    KEYWORD = 2,
    FLOAT = 3
  };

  static const size_t IMMEDIATE_STR_MAX = 7;

  /** * immediate pointer layout: [d*].lllttTTT **/
  static constexpr TagPtr MakeImmediate(uint64_t data, size_t len,
                                        IMMEDIATE_CLASS tag) {
    return static_cast<TagPtr>(((data << 8) | ((len & 0x7) << 5) |
                                ((static_cast<uint8_t>(tag) & 0x3) << 3) |
                                (static_cast<uint8_t>(TAG::IMMEDIATE) & 0x7)));
  }

  /** * immediate data (56 bits) **/
  static constexpr uint64_t ImmediateData(TagPtr ptr) {
    return static_cast<uint64_t>(ptr) >> 8;
  }

  /** * immediate size (3 bits) **/
  static constexpr size_t ImmediateSize(TagPtr ptr) {
    return static_cast<size_t>((static_cast<uint64_t>(ptr) >> 5) & 0x7);
  }

  /** * immediate class (2 bits) **/
  static constexpr IMMEDIATE_CLASS ImmediateClass(TagPtr ptr) {
    return static_cast<IMMEDIATE_CLASS>((static_cast<uint64_t>(ptr) >> 3) &
                                        0x3);
  }

  /** * constant symbol T **/
  static constexpr TagPtr T = static_cast<TagPtr>(
      ('t' << 8) | ((1 & 0x7) << 5) |
      ((static_cast<uint8_t>(IMMEDIATE_CLASS::KEYWORD) & 0x3) << 3) |
      (static_cast<uint8_t>(TAG::IMMEDIATE) & 0x7));

  /** * constant symbol NIL **/
  static constexpr TagPtr NIL = static_cast<TagPtr>(
      ((('l' << 16) | ('i' << 8) | 'n') << 8) | ((3 & 0x7) << 5) |
      ((static_cast<uint8_t>(IMMEDIATE_CLASS::KEYWORD) & 0x3) << 3) |
      (static_cast<uint8_t>(TAG::IMMEDIATE) & 0x7));

  /** * system classes **/
  enum class SYS_CLASS : uint8_t {
    ADDRESS,
    BYTE,
    CHAR,
    CODE,
    CONS,
    EXCEPTION,
    FIXNUM,
    FLOAT,
    FUNCTION,
    MACRO,
    NAMESPACE,
    STREAM,
    STRING,
    STRUCT,
    SYMBOL,
    T,
    VECTOR
  };

 public:
  static SYS_CLASS TypeOf(TagPtr);
  static bool IsClassSymbol(TagPtr);
  static TagPtr MapClassSymbol(SYS_CLASS);
  static SYS_CLASS MapSymbolClass(TagPtr);

  static constexpr TagPtr BoolOf(bool test) { return test ? T : NIL; }

  static constexpr bool IsImmediate(TagPtr ptr) {
    return TagOf(ptr) == TAG::IMMEDIATE;
  }

  static constexpr bool IsExtended(TagPtr ptr) {
    return TagOf(ptr) == TAG::EXTEND;
  }

  static constexpr bool Eq(TagPtr p0, TagPtr p1) { return p0 == p1; }
  static constexpr bool Null(TagPtr ptr) { return Eq(ptr, NIL); }

 public:       /* object model */
  TagPtr tag_; /* tagged pointer for type constructors */

  virtual TagPtr Evict(Env*, const char*) = 0;

}; /* class Type */

} /* namespace libmu */

#endif /* _LIBMU_TYPE_H_ */
