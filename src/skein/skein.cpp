/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  skein.cpp: skein main
 **
 **/
#include "skein.h"

#include <csignal>
#include <ctime>
#include <functional>
#include <iostream>
#include <mutex>
#include <tuple>
#include <thread>
#include <utility>

#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <vector>

#include "combinator.h"

namespace skein {
namespace {
  
} /* anonymous namespace */

auto Skein::options(int argc, char* argv[]) -> struct config* {
  const char* tags = "hve:r";
  
  const struct option opts[] = {
     {"eval",      required_argument, 0, 'e'},
     {"help",      no_argument,       0, 'h'},
     {"repl",      no_argument,       0, 'r'},
     {"version",   no_argument,       0, 'v'},
     {0,0,0,0},
  };

  auto cfg = new Config(false, "", "");

  for (;;) {
    auto arg = getopt_long(argc, argv, tags, opts, nullptr);

    switch (arg) {
    case 'v':
      printf("skein: v%s\n", SKEIN_VERSION);
      exit(0);
      break;
    case 'r':
      cfg->useRepl = true;
      break;
    case 'e': {
      auto expr = com_.Reduce(optarg);
      printf("%s\n", expr.c_str());
      break;
    }
    case 'h':
      printf("Usage: skein [OPTION]\n"
             "skein: BCKW combinator evaluator\n"
             "  -h, --help: print this message and exit\n"
             "  -v, --version: print version and exit\n"
             "  -e, --eval: evaluate expression\n"
             "  -r, --repl, -r: repl\n"
             );
      exit(0);
    case -1:
      return cfg;
    }
  }
}

void Skein::Repl() {
  printf("Bxyz ~> %s\n", skein::Combinator::Reduce("Bxyz").c_str());
  printf("Cxyz ~> %s\n", skein::Combinator::Reduce("Cxyz").c_str());
  printf("Kxy ~> %s\n", skein::Combinator::Reduce("Kxy").c_str());
  printf("Wxy ~> %s\n", skein::Combinator::Reduce("Wxy").c_str());

  std::string input;
  for (;;) {
    std::cout << "skein> ";
    std::cin >> input;
    printf("%s ~> %s\n", input.c_str(), skein::Combinator::Reduce(input).c_str());
  }
}
  
} /* namespace skein */
