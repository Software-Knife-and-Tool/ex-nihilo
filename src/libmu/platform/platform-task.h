/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  task.h: task abstraction
 **
 **/
#if !defined(LIBMU_PLATFORM_TASK_H_)
#define LIBMU_PLATFORM_TASK_H_

#include <cassert>
#include <cinttypes>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace libmu {
namespace platform {

class Task {
 public:
  Heap *heap_;

  Task() {}
  ~Task() {}
};

} /* namespace platform */
} /* namespace libmu */

#endif /* LIBMU_PLATFORM_TASK_H_ */
