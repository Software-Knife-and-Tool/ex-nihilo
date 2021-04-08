/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  tagfmt.h: libmu tags
 **
 **/
#if !defined(LIBMU_TAGFMT_H_)
#define LIBMU_TAGFMT_H_

#include <algorithm>
#include <cassert>
#include <cinttypes>
#include <cstdio>
#include <iostream>
#include <list>
#include <string>
#include <utility>

#include "libmu/heap/heap.h"
#include "libmu/platform/platform.h"
#include "libmu/type.h"

namespace libmu {
namespace core {

class Env;

using platform::Platform;

using SYS_CLASS = core::Type::SYS_CLASS;
using Tag = core::Type::Tag;
using HeapInfo = heap::Heap::HeapInfo;

template <typename H>
class TagFmt {
 public:
  typedef std::vector<uint64_t> HeapFormat;

 private:
  HeapFormat* tagFmt_;
  Tag tag_;

 public:
  static constexpr size_t HeapWords(size_t nbytes) { return (nbytes + 7) / 8; }

  /** * HeapInfo from Tag **/
  static heap::Heap::HeapInfo* GetHeapInfo(Tag ptr) {
    return Type::Untag<HeapInfo>(ptr) - 1;
  }

  /** * SYS_CLASS from tag */
  static SYS_CLASS SysClass(Tag ptr) {
    auto hinfo = *GetHeapInfo(ptr);

    return heap::Heap::SysClass(static_cast<HeapInfo>(hinfo));
  }

  /** * dump tag format **/
  template <typename T>
  void DumpTag(Tag ptr) {
    if (!core::Type::IsImmediate(ptr)) {
      printf("HeapImage: tag 0x%016llx", ptr);
      auto heapInfo = GetHeapInfo(ptr);
      auto size = heap::Heap::Size(*heapInfo);
      // auto refbits = heap::Heap::RefBits(*heapInfo);
      // auto reloc = heap::Heap::Reloc(*heapInfo);
      auto sys_class = heap::Heap::SysClass(*heapInfo);

      heap::Heap::Dump(*heapInfo);
      printf("image: %u(%lu words), layout %lu(%lu words)\n", size,
             HeapWords(size), sizeof(T), HeapWords(sizeof(T)));
      printf("system class: %s\n", core::Type::SysClassOf(sys_class).c_str());
      for (uint32_t i = 0; i < HeapWords(size); ++i)
        if (static_cast<uint64_t>(heapInfo[i + 1]))
          printf("% 2d: 0x%016llx\n", i, heapInfo[i + 1]);
    }
  }

  /** * evict format **/
  template <typename T>
  void Evict(Tag ptr) {
    if (!core::Type::IsImmediate(ptr)) {
      printf("HeapImage: tag 0x%016llx", ptr);
      auto heapInfo = GetHeapInfo(ptr);
      auto size = heap::Heap::Size(*heapInfo);
      // auto refbits = heap::Heap::RefBits(*heapInfo);
      // auto reloc = heap::Heap::Reloc(*heapInfo);
      auto sys_class = heap::Heap::SysClass(*heapInfo);

      heap::Heap::Dump(*heapInfo);
      printf("image: %u(%lu words), layout %lu(%lu words)\n", size,
             HeapWords(size), sizeof(T), HeapWords(sizeof(T)));
      printf("system class: %s\n", core::Type::SysClassOf(sys_class).c_str());
      for (uint32_t i = 0; i < HeapWords(size); ++i)
        if (static_cast<uint64_t>(heapInfo[i + 1]))
          printf("% 2d: 0x%016llx\n", i, heapInfo[i + 1]);
    }
  }

  /** * evict tag format **/
  template <typename T>
  void EvictTag(Tag ptr) {
    if (!core::Type::IsImmediate(ptr)) {
      printf("HeapImage: tag 0x%016llx", ptr);
      auto heapInfo = GetHeapInfo(ptr);
      auto size = heap::Heap::Size(*heapInfo);
      // auto refbits = heap::Heap::RefBits(*heapInfo);
      // auto reloc = heap::Heap::Reloc(*heapInfo);
      auto sys_class = heap::Heap::SysClass(*heapInfo);

      heap::Heap::Dump(*heapInfo);
      printf("image: %u(%lu words), layout %lu(%lu words)\n", size,
             HeapWords(size), sizeof(T), HeapWords(sizeof(T)));
      printf("system class: %s\n", core::Type::SysClassOf(sys_class).c_str());
      for (uint32_t i = 0; i < HeapWords(size); ++i)
        if (static_cast<uint64_t>(heapInfo[i + 1]))
          printf("% 2d: 0x%016llx\n", i, heapInfo[i + 1]);
    }
  }

 public: /* tag */
  explicit TagFmt(size_t extra, Tag ptr) {
    auto sc = SysClass(ptr);
    size_t nalloc = sizeof(heap::Heap::HeapInfo) + HeapWords(sizeof(H)) * 8;

    tagFmt_ =
        new std::vector<uint64_t>(1 + HeapWords(sizeof(H)) + HeapWords(extra));

    tagFmt_->at(0) =
        static_cast<uint64_t>(heap::Heap::MakeHeapInfo(nalloc, sc));

    auto hi = reinterpret_cast<H*>(tagFmt_->data() + 1);

    *hi = *reinterpret_cast<H*>(Type::Untag<uint64_t>(ptr));
    tag_ = Type::Entag(tagFmt_->data() + 1, Type::TAG::EXTEND);
  }

} /* class TagFmt */;

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TAGFMT_H_ */
