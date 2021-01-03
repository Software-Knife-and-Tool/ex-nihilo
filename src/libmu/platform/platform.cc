/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 ** platform.cc: platform functions
 **
 **/
#include <cassert>
#include <cstdio>
#include <ctime>

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include "libmu/platform/platform-ffi.h"
#include "libmu/platform/platform.h"

extern char **environ;

namespace libmu {
namespace platform {

/** * map pages in system address space **/
const char *Platform::MapPages(unsigned npages, const char *heapId) {
  char *base;
  int fd, seek;
  char tmpfn[PATH_MAX];

  strcpy(tmpfn, "/tmp/lambda-");
  strcat(tmpfn, heapId);
  strcat(tmpfn, "-XXXXXX");

  fd = mkstemp(tmpfn);
  assert(fd >= 0);

  seek = ftruncate(fd, npages * PAGESIZE);
  assert(seek >= 0);

  base = (char *)mmap(NULL, npages * PAGESIZE, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE, fd, 0);

  assert(base != (char *)-1);
  close(fd);

  unlink(tmpfn);

  return base;
}

/** * get system clock time in millseconds**/
void Platform::SystemTime(uint64_t *retn) {
  struct timeval now;

  assert(gettimeofday(&now, NULL) >= 0);
  *retn = (now.tv_sec * 1e6) + (now.tv_usec / 1000);
}

/** * get process elapsed time in milliseconds**/
void Platform::ProcessTime(uint64_t *retn) {
  struct timespec now;

  /* check return, CLOCK_PROCESS_CPUTIME_ID may not be portable */
  assert(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now) >= 0);
  *retn = (now.tv_sec * 1e6) + (now.tv_nsec / 1e3);
}

/** * system **/
int Platform::System(const std::string cmd) { return system(cmd.c_str()); }

/** * system environment **/
char **Platform::Environment() { return environ; }

/** * platform invoke **/
std::string Platform::Invoke(uint64_t fnp, std::string arg) {
  /* I ought to be slapped for this */
  std::string (*fn)(std::string) = (std::string(*)(std::string))(fnp);

  return fn(arg);
}
} /* namespace platform */
} /* namespace libmu */
