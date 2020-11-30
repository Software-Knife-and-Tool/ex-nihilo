/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  compiler.h: library form compiler
 **
 **/
#if !defined(_LIBMU_COMPILER_H_)
#define _LIBMU_COMPILER_H_

#include <cassert>

#include "libmu/env.h"
#include "libmu/type.h"

#include "libmu/types/cons.h"

namespace libmu {

class Compiler : public Type {
 private: /* singleton */
  Compiler(){};

 public:
  static TagPtr Compile(Env*, TagPtr);
  static TagPtr CompileLambda(Env*, TagPtr);
  static TagPtr CompileLexical(Env*, TagPtr, size_t);
  static TagPtr CompileList(Env*, TagPtr);
  static TagPtr CompileSpecial(Env*, TagPtr);

  static TagPtr DefConstant(Env*, TagPtr);
  static TagPtr DefMacro(Env*, TagPtr);
  static TagPtr Environ(Env*, TagPtr);
  static TagPtr Lambda(Env*, TagPtr);
  static TagPtr Letq(Env*, TagPtr);
  static TagPtr ParseLambda(Env*, TagPtr);
  static TagPtr Quote(Env*, TagPtr);

  static bool InLexicalEnv(Env*, TagPtr, TagPtr*, size_t*);
  static bool IsSpecOp(Env*, TagPtr);

  static constexpr TagPtr lexicals(TagPtr lambda) {
    assert(Cons::IsList(lambda));

    return Cons::car(lambda);
  }

  static constexpr TagPtr restsym(TagPtr lambda) {
    assert(Cons::IsList(lambda));

    return Cons::cdr(lambda);
  }

}; /* class Compiler */

} /* namespace libmu */

#endif /* _LIBMU_COMPILER_H_ */
