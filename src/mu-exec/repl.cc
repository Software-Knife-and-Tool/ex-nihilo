/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  repl.cc: mu-exec repl
 **
 **/
#include <iostream>
#include <sstream>
#include <string>

#include "libmu/platform/platform.h"

#include "libmu/libmu.h"

using libmu::platform::Platform;

void repl(Platform *platform, int) {
  auto env = libmu::api::env_default(platform);

  libmu::api::withException(env, [platform](void *env) {
    auto repl = false;

    for (const Platform::OptMap &opt : *platform->options_) {
      switch (platform->name(opt)[0]) {
      case '?': /* fall through */
      case 'h': {
        const char *helpmsg =
            "OVERVIEW: mu-exec - posix platform mu repl\n"
            "USAGE: mu-exec [options] [src-file...]\n"
            "\n"
            "OPTIONS:\n"
            "  -h                   print this message\n"
            "  -v                   print version string\n"
            "  -i                   enter repl\n"
            "  -l SRCFILE           load SRCFILE in sequence\n"
            "  -e SEXPR             evaluate SEXPR and print result\n"
            "  -q SEXPR             evaluate SEXPR quietly\n"
            "  src-file             load source file\n";

        std::cout << helpmsg << std::endl;
        if (platform->name(opt)[0] == '?') {
          return;
        }
        break;
      }
      case 'i':
        repl = true;
        break;
      case 'l': {
        auto cmd = "(load \"" + platform->value(opt) + "\")";
        (void)libmu::api::eval(env, libmu::api::read_string(env, cmd));
        break;
      }
      case 'q':
        (void)libmu::api::eval(
            env, libmu::api::read_string(env, platform->value(opt)));
        break;
      case 'e':
        libmu::api::print(env,
                          libmu::api::eval(env, libmu::api::read_string(
                                                    env, platform->value(opt))),
                          libmu::api::nil(), false);
        libmu::api::terpri(env, libmu::api::nil());
        break;
      case 'v':
        std::cout << libmu::api::version() << std::endl;
        break;
      }
    }

    /* load files from command line */
    for (const std::string &file : *platform->optargs_) {
      auto cmd = "(load \"" + file + "\")";
      (void)libmu::api::eval(env, libmu::api::read_string(env, cmd));
    }

    if (repl)
      for (;;) {
        libmu::api::withException(env, [](void *env) {
          if (feof(stdin))
            exit(0);

          libmu::api::print(env,
                            libmu::api::eval(env, libmu::api::read_stream(
                                                      env, libmu::api::t())),
                            libmu::api::nil(), false);
          libmu::api::terpri(env, libmu::api::nil());
        });
      }
  });
}
