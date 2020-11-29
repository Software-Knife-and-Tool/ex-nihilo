/*******
 **
 ** Copyright (c) 2017, James M. Putnam
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are met:
 **
 ** 1. Redistributions of source code must retain the above copyright notice,
 **    this list of conditions and the following disclaimer.
 **
 ** 2. Redistributions in binary form must reproduce the above copyright
 **    notice, this list of conditions and the following disclaimer in the
 **    documentation and/or other materials provided with the distribution.
 **
 ** 3. Neither the name of the copyright holder nor the names of its
 **    contributors may be used to endorse or promote products derived from
 **    this software without specific prior written permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 ** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 ** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 ** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 ** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 ** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 ** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 ** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 ** POSSIBILITY OF SUCH DAMAGE.
 **
 *******/

/********
 **
 **  repl.cc: mu-repl repl
 **
 **/
#include <iostream>
#include <sstream>
#include <string>

#include "platform/platform.h"

#include "libmu/libmu.h"

using platform::Platform;

void repl(Platform *platform, int) {

  auto env = libmu_env_default(platform);
  
  libmu_withException(env, [platform](void* env) {
    for (const Platform::OptMap &opt : *platform->options_) {
      switch (platform->name(opt)[0]) {
      case '?': /* fall through */
      case 'h': {
        const char *helpmsg =
          "OVERVIEW: mu-repl - posix platform mu repl\n"
          "USAGE: mu-repl [options] [src-file...]\n"
          "\n"
          "OPTIONS:\n"
          "  -h                   print this message\n"
          "  -v                   print version string\n"
          "  -l SRCFILE           load SRCFILE in sequence\n"
          "  -e SEXPR             evaluate SEXPR and print result\n"
          "  -q SEXPR             evaluate SEXPR quietly\n"
          "  src-file             load source file\n";
        
        std::cout << helpmsg << std::endl;
        if (platform->name(opt)[0] == '?') { return; }
        break;
      }
      case 'l': {
        auto cmd = "(load \"" + platform->value(opt) + "\")";
        (void)libmu_eval(env, libmu_read_string(env, cmd));
        break;
      }
      case 'q':
        (void)libmu_eval(env,
                         libmu_read_string(env,
                                           platform->value(opt)));
        break;
      case 'e':
        libmu_print(env,
                    libmu_eval(env,
                               libmu_read_string(env,
                                                 platform->value(opt))),
                    libmu_nil(),
                    false);
        libmu_terpri(env, libmu_nil());
        break;
      case 'v':
        std::cout << libmu_version() << std::endl;
        break;
      }
    }

    /* load files from command line */
    for (const std::string& file : *platform->optargs_) {
      auto cmd = "(load \"" + file + "\")";
      (void)libmu_eval(env, libmu_read_string(env, cmd));
    }
  });

  for (;;) {
    libmu_withException(env, [](void* env) {
      if (feof(stdin)) { exit(0); }
      
      libmu_print(env,
                  libmu_eval(env,
                             libmu_read_stream(env,
                                               libmu_t())),
                  libmu_nil(),
                  false);
      libmu_terpri(env, libmu_nil());
    });
  }
}
