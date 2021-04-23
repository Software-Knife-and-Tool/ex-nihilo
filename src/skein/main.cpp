/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  main.cpp: entrez-vous
 **
 **/
#include "skein.h"

#include <csignal>
#include <ctime>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <utility>

#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <vector>

/** * entry point **/
int main(int argc, char* argv[]) {

  auto root = new skein::Skein(argc, argv);

  if (root->cfg_->useRepl)
    root->Repl();
  return 0;
}
