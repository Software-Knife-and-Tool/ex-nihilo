/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu-heap.cc: mu heap functions
 **
 **/
#include <cassert>
#include <numeric>

#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "platform/platform.h"

#include "libmu/print.h"
#include "libmu/type.h"

#include "libmu/heap/heap.h"
#include "libmu/types/exception.h"

namespace libmu {
namespace mu {

/** * (gc bool) => fixnum **/
void Gc(Env::Frame* fp) {
  auto arg = fp->argv[0];

  switch (arg) {
    case Type::NIL:
    case Type::T:
      break;
    default:
      Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                       "is not boolean (gc)", arg);
  }

  fp->value = Fixnum(fp->env->Gc(fp->env)).tag_;
}

/** * mu function (heap-log bool) => :nil **/
void HeapLog(Env::Frame* fp) {
  fp->value = fp->argv[0];

  switch (fp->value) {
    case Type::NIL:
    case Type::T:
      break;
    default:
      Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                       "is not boolean (heap-log)", fp->value);
  }

  fp->env->heap_->logging(Type::Null(fp->value) ? false : true);
}

/** * mu function (heap-info type) => vector **/
void HeapInfo(Env::Frame* fp) {
  auto type = fp->argv[0];

  if (!Symbol::IsKeyword(type) || !Type::IsClassSymbol(type))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "is not a system class keyword (heap-info)", type);

  std::function<TagPtr(Type::SYS_CLASS, int)> type_vec =
      [fp](Type::SYS_CLASS sys_class, int size) {
        return Vector(fp->env,
                      std::vector<TagPtr>{
                          Fixnum(-1).tag_, /* figure out per object size */
                          Fixnum(size).tag_,
                          Fixnum(fp->env->heap_->nalloc_->at(
                                     static_cast<int>(sys_class)))
                              .tag_,
                          Fixnum(fp->env->heap_->nfree_->at(
                                     static_cast<int>(sys_class)))
                              .tag_})
            .tag_;
      };

  /** * immediates return :nil */
  auto sys_class = Type::MapSymbolClass(type);
  switch (sys_class) {
    case Type::SYS_CLASS::BYTE:
    case Type::SYS_CLASS::CHAR:
    case Type::SYS_CLASS::FIXNUM:
    case Type::SYS_CLASS::FLOAT:
      fp->value = Type::NIL;
      break;
    case Type::SYS_CLASS::T:
      fp->value =
          Vector(fp->env,
                 std::vector<TagPtr>{
                     Fixnum(fp->env->heap_->size()).tag_,
                     Fixnum(fp->env->heap_->alloc()).tag_,
                     Fixnum(std::accumulate(fp->env->heap_->nalloc_->begin(),
                                            fp->env->heap_->nalloc_->end(), 0))
                         .tag_,
                     Fixnum(std::accumulate(fp->env->heap_->nfree_->begin(),
                                            fp->env->heap_->nfree_->end(), 0))
                         .tag_})
              .tag_;
      break;
    default:
      fp->value = type_vec(sys_class, fp->env->heap_->room(sys_class));
      break;
  }
}

} /* namespace mu */
} /* namespace libmu */
