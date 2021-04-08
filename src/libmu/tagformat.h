/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  tagformat.h: libmu tags
 **
 **/
#if !defined(LIBMU_TAGFORMAT_H_)
#define LIBMU_TAGFORMAT_H_

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
class TagFormat {
 public:
  typedef std::vector<uint64_t> HeapFormat;
  Tag tag_;

 private:
  HeapFormat* tagFormat_;

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

  /** * heap object marked? **/
  static auto IsGcMarked(Tag ptr) -> bool {
    auto hinfo = GetHeapInfo(ptr);

    return heap::Heap::RefBits(*hinfo) == 0 ? false : true;
  }

  /** * mark heap object **/
  static auto GcMark(Tag ptr) -> void {
    auto hinfo = GetHeapInfo(ptr);

    *hinfo = heap::Heap::RefBits(*hinfo, 1);
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
  void Evict(Tag) {
    // assert(!IsEvicted(ptr));
  }

  /** * evict tag format **/
  void EvictTag(Tag) {
    // assert(!IsEvicted(ptr));
  }

 public: /* tag */
  explicit TagFormat(Type::SYS_CLASS sc, Type::TAG tag, H* layout) {
    size_t nalloc = sizeof(heap::Heap::HeapInfo) + HeapWords(sizeof(H)) * 8;

    tagFormat_ = new std::vector<uint64_t>(1 + HeapWords(sizeof(H)));

    tagFormat_->at(0) =
        static_cast<uint64_t>(heap::Heap::MakeHeapInfo(nalloc, sc));

    auto hi = reinterpret_cast<H*>(tagFormat_->data() + 1);

    *hi = *layout;
    tag_ = Type::Entag(tagFormat_->data() + 1, tag);
  }

} /* class TagFormat */;

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_TAGFORMAT_H_ */
