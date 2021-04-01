/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  mu-heap.cc: mu heap functions
 **
 **/
#include <cassert>
#include <numeric>

#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "libmu/platform/platform.h"

#include "libmu/core.h"
#include "libmu/type.h"

#include "libmu/heap/heap.h"
#include "libmu/types/condition.h"

namespace libmu {
namespace mu {

using Condition = core::Condition;
using Fixnum = core::Fixnum;
using Frame = core::Env::Frame;
using Type = core::Type;

/** * (gc bool) => fixnum **/
auto Gc(Frame* fp) -> void {
  auto arg = fp->argv[0];

  switch (arg) {
    case Type::NIL:
    case Type::T:
      break;
    default:
      Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                       "is not boolean (gc)", arg);
  }

  fp->value = core::Fixnum(fp->env->Gc(fp->env)).tag_;
}

/** * (heap-info type) => vector **/
auto HeapInfo(Frame* fp) -> void {
  auto type = fp->argv[0];

  if (!core::Symbol::IsKeyword(type) || !Type::IsClassSymbol(type))
    Condition::Raise(fp->env, Condition::CONDITION_CLASS::TYPE_ERROR,
                     "is not a system class keyword (heap-info)", type);

  std::function<Type::Tag(size_t, Type::SYS_CLASS)> type_vec =
      [fp](size_t size, Type::SYS_CLASS sys_class) {
        return core::Vector<Type::Tag>(
                   std::vector<Type::Tag>{
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
          core::Vector<Type::Tag>(
              std::vector<Type::Tag>{
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
      fp->value = type_vec(fp->env->heap_->room(sys_class), sys_class);
      break;
  }
}

} /* namespace mu */
} /* namespace libmu */
