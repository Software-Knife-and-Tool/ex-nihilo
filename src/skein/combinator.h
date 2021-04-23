/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  combinator.h: combinator class
 **
 **/
#if !defined(SKEIN_COMBINATOR_H_)
#define SKEIN_COMBINATOR_H_

#include <csignal>
#include <ctime>
#include <cstdint>
#include <functional>
#include <iostream>
#include <mutex>
#include <tuple>
#include <thread>
#include <utility>
#include <vector>

#include <getopt.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

namespace skein {
    
class Combinator {
 public:
  class Term {
    public:
      bool is_symbol;
      uint32_t length;
      char symbol;
      std::pair<Term*, Term*> pair;

      std::string StringOf() {
        return is_symbol ? std::string{symbol} :
          "(" + pair.first->StringOf() + pair.second->StringOf() + ")";
      }

      Term() { }
      Term(std::string source) {
        switch (source.at(0)) {
        case '(': {
          is_symbol = false;
          auto t1 = new Term(source.substr(1));
          auto t2 = new Term(source.substr(1 + t1->length));
          length = t1->length + t2->length + 2;
          pair = std::pair<Term*, Term*>{t1, t2};
          break;
        }
        default:
          is_symbol = true;
          length = 1;
          symbol = source.at(0);
          break;
        }
      }
  }; /* class Term */

 public: /* object model */
  static std::pair<std::string, std::string> ReduceOnce(Term, std::string);
  static std::string Reduce(std::string);

  Combinator() { }

}; /* class Skein */

} /* skein namespace */

#endif /* SKEIN_COMBINATOR_H_ */
