/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  opts.cc: posix run-time options
 **
 **/
#include <getopt.h>

#include "libmu/platform/platform.h"

using libmu::platform::Platform;

void make_opts(Platform *platform, int argc, char **argv, const char *flags) {
  int opt;
  extern char *optarg;
  extern int optind;

  while ((opt = getopt(argc, argv, flags)) != -1) {
    platform->PushOpt(opt, optarg);
  }

  for (auto i = optind; i < argc; ++i) {
    platform->PushArg(argv[i]);
  }
}
