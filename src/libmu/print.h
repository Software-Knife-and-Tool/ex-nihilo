/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
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

using TagPtr = Type::TagPtr;

void Print(Env*, TagPtr, TagPtr, bool);
void PrintStdString(Env*, const std::string&, TagPtr, bool);
void Terpri(Env*, TagPtr);

} /* namespace libmu */

#endif /* _LIBMU_PRINT_H_ */
