/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  heap.cc: library heap
 **
 **/
#include "libmu/heap/heap.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cassert>

#include "libmu/platform/platform.h"

#include "libmu/env.h"

#include "libmu/core.h"
#include "libmu/type.h"

#include "libmu/types/string.h"
#include "libmu/types/symbol.h"

namespace libmu {
namespace heap {

using SYS_CLASS = core::Type::SYS_CLASS;

/** * find free object **/
auto Heap::FindFree(size_t nbytes, SYS_CLASS tag) -> HeapInfo* {
  if (nfree_->at(static_cast<size_t>(tag)) != 0) {
    HeapInfo hinfo;

    if (tag == SYS_CLASS::CONS && conses_ != nullptr) {
      auto cp = conses_;
      auto hi = reinterpret_cast<HeapInfo**>(conses_);

      *conses_ = RefBits(*conses_, 1);
      conses_ = hi[1];

      nfree_->at(static_cast<size_t>(tag))--;
      return cp;
    }

    for (auto hp = reinterpret_cast<uint64_t>(uaddr_);
         hp < reinterpret_cast<uint64_t>(alloc_);
         hp += Size(*reinterpret_cast<HeapInfo*>(hp))) {
      hinfo = *reinterpret_cast<HeapInfo*>(hp);
      /* install a size delta check here */
      if (tag == SysClass(hinfo) && Size(hinfo) >= (nbytes + 8) &&
          RefBits(hinfo) == 0) {
        *reinterpret_cast<HeapInfo*>(hp) = RefBits(hinfo, 1);
        nfree_->at(static_cast<size_t>(tag))--;
        return reinterpret_cast<HeapInfo*>(hp);
      }
    }
  }

  return nullptr;
}

/** * allocate heap object **/
auto Heap::Alloc(size_t nbytes, SYS_CLASS tag) -> void* {
  auto fp = FindFree(nbytes, tag);

  if (fp == nullptr) {
    char* halloc = alloc_;
    size_t nalloc = sizeof(HeapInfo) + ((nbytes + 7) & ~7);

    alloc_ += nalloc;

    if (alloc_ > uaddr_ + (pagesz_ * npages_)) assert(!"heap capacity botch");

    *reinterpret_cast<HeapInfo*>(halloc) = MakeHeapInfo(nalloc, tag);

    nobjects_++;
    nalloc_->at(static_cast<size_t>(tag))++;

    return reinterpret_cast<void*>(halloc + sizeof(HeapInfo));
  } else {
    return reinterpret_cast<void*>(reinterpret_cast<char*>(fp) +
                                   sizeof(HeapInfo));
  }
}

/** * count up total data bytes in heap **/
auto Heap::room() -> size_t {
  size_t nbytes = 0;
  HeapInfo hinfo;

  for (auto hp = reinterpret_cast<uint64_t>(uaddr_);
       hp < reinterpret_cast<uint64_t>(alloc_);
       hp += Size(*reinterpret_cast<HeapInfo*>(hp))) {
    hinfo = *reinterpret_cast<HeapInfo*>(hp);
    nbytes += Size(hinfo);
  }

  return nbytes;
}

/** * count up total type tag size **/
auto Heap::room(SYS_CLASS tag) -> size_t {
  size_t total_size = 0;
  HeapInfo hinfo;

  for (auto hp = reinterpret_cast<uint64_t>(uaddr_);
       hp < reinterpret_cast<uint64_t>(alloc_);
       hp += Size(*reinterpret_cast<HeapInfo*>(hp))) {
    hinfo = *reinterpret_cast<HeapInfo*>(hp);
    if (tag == SysClass(hinfo)) total_size += Size(hinfo);
  }

  return total_size;
}

/** * heap object marked? **/
auto Heap::IsGcMarked(Tag ptr) -> bool {
  auto hinfo = GetHeapInfo(ptr);

  return RefBits(*hinfo) == 0 ? false : true;
}

/** * mark heap object **/
auto Heap::GcMark(Tag ptr) -> void {
  auto hinfo = GetHeapInfo(ptr);

  *hinfo = RefBits(*hinfo, 1);
}

/** * clear refbits **/
auto Heap::ClearRefBits() -> void {
  HeapInfo hinfo;

  for (auto hp = reinterpret_cast<uint64_t>(uaddr_);
       hp < reinterpret_cast<uint64_t>(alloc_);
       hp += Size(*reinterpret_cast<HeapInfo*>(hp))) {
    hinfo = *reinterpret_cast<HeapInfo*>(hp);
    *reinterpret_cast<HeapInfo*>(hp) = RefBits(hinfo, 0);
  }

  for (auto& fp : *nfree_) fp = 0;
}

/** * garbage collection **/
auto Heap::Gc() -> size_t {
  HeapInfo hinfo;

  size_t nmarked = 0;
  size_t nobjects = 0;

  conses_ = nullptr;

  /* count the ref bits */
  for (auto hp = reinterpret_cast<uint64_t>(uaddr_);
       hp < reinterpret_cast<uint64_t>(alloc_);
       hp += Size(*reinterpret_cast<HeapInfo*>(hp))) {
    hinfo = *reinterpret_cast<HeapInfo*>(hp);
    if (RefBits(hinfo) == 0) {
      nfree_->at(static_cast<size_t>(SysClass(hinfo)))++;
      if (SysClass(hinfo) == SYS_CLASS::CONS) {
        auto hi = reinterpret_cast<HeapInfo**>(hp);

        hi[1] = conses_;
        conses_ = reinterpret_cast<HeapInfo*>(hp);
      }
    } else {
      nobjects++;
      nmarked += Size(hinfo) + sizeof(HeapInfo);
    }
  }

  nobjects_ = nobjects;
  return (pagesz_ * npages_) - nmarked;
}

/** * heap object **/
Heap::Heap() {
  const char* heapId = "heap";

  pagesz_ = 4096;
  npages_ = 1024 * 16;

  uaddr_ = const_cast<char*>(Platform::MapPages(npages_, heapId));
  alloc_ = uaddr_;
  conses_ = nullptr;
  nfree_ = new std::vector<uint32_t>(256, 0);
  nalloc_ = new std::vector<uint32_t>(256, 0);
  filename_ = "";
}

} /* namespace heap */
} /* namespace libmu */
