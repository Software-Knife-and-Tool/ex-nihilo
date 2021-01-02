/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  print.h: libmu printing
 **
 **/

#if !defined(_LIBMU_PRINT_H_)
#define _LIBMU_PRINT_H_

#include "libmu/type.h"

namespace libmu {
namespace core {

using Tag = Type::Tag;

void Print(Env*, Tag, Tag, bool);
void PrintStdString(Env*, const std::string&, Tag, bool);
void Terpri(Env*, Tag);

} /* namespace core */
} /* namespace libmu */

#endif /* _LIBMU_PRINT_H_ */
