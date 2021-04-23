/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  combinator.cpp: combinator class
 **
 **/
#include "combinator.h"

#include <cassert>
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

namespace skein {
namespace {

using Term = Combinator::Term;
  
std::tuple<Term, Term> term2(std::string source) {
  Term x = Term(source);
  Term y = Term(source.substr(x.length));

  return std::tuple<Term, Term>{x, y};
}

std::tuple<Term, Term, Term> term3(std::string source) {
  Term x = Term(source);
  Term y = Term(source.substr(x.length));
  Term z = Term(source.substr(x.length + y.length));

  return std::tuple<Term, Term, Term>{x, y, z};
}

} /* anonymous namespace */
  
std::pair<std::string, std::string> Combinator::ReduceOnce(Term combinator, std::string args) {
  assert(combinator.is_symbol);

  Term x, y, z;

  switch (combinator.symbol) {
  case 'B': /* xyz ~> x(yz) */
    std::tie(x, y, z) = term3(args);
    return std::pair<std::string, std::string>{
      x.StringOf() + "(" + y.StringOf() + z.StringOf() + ")",
      args.substr(x.length + y.length + z.length)};
    break;
  case 'C': /* xyz ~> xzy */
    std::tie(x, y, z) = term3(args);
    return std::pair<std::string, std::string>{
      x.StringOf() + z.StringOf() + y.StringOf(),
      args.substr(x.length + y.length + z.length)};
    break;
  case 'K': /* xy ~> x */
    std::tie(x, y) = term2(args);
    return std::pair<std::string, std::string>{
      x.StringOf(),
      args.substr(x.length + y.length)};
    break;
  case 'W': /* xy ~> xyy */
    std::tie(x, y) = term2(args);
    return std::pair<std::string, std::string>{
      x.StringOf() + y.StringOf() + y.StringOf(),
      args.substr(x.length + y.length)};
    break;
  default:
    return std::pair<std::string, std::string>{"", ""};
  }
}

/** * reduce **/
std::string Combinator::Reduce(std::string source) {
  Term combinator = Term(source);
  std::string reduction = "";
  std::string form = source.substr(1);

  for (;;) {
    std::tie(reduction, form) = ReduceOnce(combinator, form);
    if (form.length() == 0)
      return reduction;

    form = reduction + form;
    combinator = Term(std::string{form.at(0)});
    form = form.substr(1);
  }
}

} /* namespace skein */
