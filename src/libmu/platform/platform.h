/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  platform.h: platform abstraction
 **
 **/
#include <algorithm>
#include <cassert>
#include <cinttypes>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#if !defined(LIBMU_PLATFORM_PLATFORM_H_)
#define LIBMU_PLATFORM_PLATFORM_H_

namespace libmu {
namespace platform {

class Platform {
 public: /* options */
  typedef std::pair<const std::string, const std::string> OptMap;
  typedef std::list<OptMap>::iterator OptMapPtr;

  std::unique_ptr<std::list<OptMap>> options_;
  std::unique_ptr<std::list<std::string>> optargs_;

  OptMapPtr next_;

  void PushArg(const char *arg) { optargs_->push_back(arg); }

  void PushOpt(int key, const char *value) {
    char cvec[2];
    cvec[0] = key;
    cvec[1] = '\0';
    options_->push_back(std::make_pair(cvec, value == NULL ? "" : value));
  }

  OptMapPtr Find(const std::string key) {
    next_ = options_->begin();
    return std::find_if(options_->begin(), options_->end(), [key](OptMap el) {
      return el.first.compare(key) == 0;
    });
  }

  OptMapPtr FindNext(const std::string key) {
    return std::find_if(next_, options_->end(), [key](OptMap el) {
      return el.first.compare(key) == 0;
    });
  }

  bool IsFound(OptMapPtr el) { return el != options_->end(); }

  /* accessors */
  static std::string name(OptMapPtr el) { return el->first; }
  static std::string value(OptMapPtr el) { return el->second; }

  static std::string name(OptMap el) { return el.first; }
  static std::string value(OptMap el) { return el.second; }

 public: /* persistance */
  static const int PAGESIZE = 4096;
  static const char *MapPages(unsigned, const char *);

 public: /* streams */
  typedef int64_t StreamId;
  static const StreamId STREAM_ERROR = -1;

  enum STD_STREAM { STDIN, STDOUT, STDERR };

  static bool IsClosed(StreamId);
  static bool IsEof(StreamId);
  static bool IsFile(StreamId);
  static bool IsInput(StreamId);
  static bool IsOutput(StreamId);
  static bool IsStdStream(StreamId);
  static bool IsString(StreamId);

  static void Close(StreamId);

  static StreamId AcceptSocketStream(StreamId);
  static StreamId ConnectSocketStream(StreamId);
  static StreamId OpenClientSocketStream(int, int);
  static StreamId OpenInputFile(std::string);
  static StreamId OpenInputString(std::string);
  static StreamId OpenOutputFile(std::string);
  static StreamId OpenOutputString(std::string);
  static StreamId OpenServerSocketStream(int);
  static StreamId OpenStandardStream(Platform::STD_STREAM);

  static std::string GetStdString(StreamId);
  static void Flush(StreamId);

  static int ReadByte(StreamId);
  static int UnReadByte(int, StreamId);
  static void WriteByte(int, StreamId);

 public: /* time */
  static void SystemTime(uint64_t*);
  static void ProcessTime(uint64_t*);

 public: /* tasks */
  static void TaskCreate();
  static void TaskKill();

 public: /* threads */
 public: /* generalize conditions */
 public: /* generalize ffi */
  static char **Environment();
  static int System(const std::string);
  static std::string Invoke(uint64_t, std::string);

  Platform()
      : options_(std::make_unique<std::list<OptMap>>()),
        optargs_(std::make_unique<std::list<std::string>>()),
        next_(options_->begin()) {}

  ~Platform() {}
};
} /* namespace platform */
} /* namespace libmu */

#endif /* LIBMU_PLATFORM_PLATFORM_H_ */
