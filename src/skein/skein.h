/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  skein.h: skein class
 **
 **/
#if !defined(SKEIN_SKEIN_H_)
#define SKEIN_SKEIN_H_

#include <csignal>
#include <ctime>
#include <cstdint>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#include <getopt.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "combinator.h"

namespace skein {

class Skein {
 public:
  typedef struct config {
    config(bool useRepl,
           std::string path,
           std::string expr)
      : useRepl(useRepl),
        path(path),
        expr(expr)
    {}
  
    ~config() {}
  
    bool useRepl;
    std::string path;
    std::string expr;
  } Config;

  static constexpr auto SKEIN_VERSION = "0.0.1";

  void Repl();

 private:
  Config* options(int, char*[]);

 public: /* object model */
  Config* cfg_;
  Combinator com_;
  
  Skein(int argc, char* argv[]) : cfg_(options(argc, argv)), com_(Combinator()) { }

}; /* class Skein */

} /* skein namespace */

#endif /* SKEIN_SKEIN_H_ */
