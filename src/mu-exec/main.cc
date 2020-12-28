/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  main.cc: posix main
 **
 **/
#include "libmu/platform/platform.h"

using libmu::platform::Platform;

extern void repl(Platform *, int);
extern void make_opts(Platform *, int, char **, const char *);

int main(int argc, char **argv) {
  Platform *platform = new Platform();

  make_opts(platform, argc, argv, "ivhe:q:l:");
  repl(platform, argc);

  return 0;
}
