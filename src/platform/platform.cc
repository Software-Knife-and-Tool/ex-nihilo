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
 ** platform.cc: platform functions
 **
 **/
#include <cassert>
#include <cstdio>

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "platform/ffi.h"
#include "platform/platform.h"

extern char** environ;

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

/** * get system clock time **/
void Platform::SystemTime(unsigned long *retn) {
  struct timeval now;

  assert(gettimeofday(&now, NULL) >= 0);
  retn[0] = now.tv_sec;
  retn[1] = now.tv_usec;
}

/** * get process elapsed time **/
void Platform::ProcessTime(unsigned long *retn) {
  struct timespec now;

  /* check return, CLOCK_PROCESS_CPUTIME_ID may not be portable */
  assert(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now) >= 0);

  retn[0] = now.tv_sec;
  retn[1] = now.tv_nsec / 100;
}

/** * system **/
int Platform::System(const std::string cmd) {

  return system(cmd.c_str());
}

/** * system environment **/
char** Platform::Environment() {
  
  return environ;
}

/** * platform invoke **/
std::string Platform::Invoke(uint64_t fnp, std::string arg) {

  /* I ought to be slapped for this */
  std::string (* fn)(std::string) = (std::string (*)(std::string))(fnp);

  return fn(arg);
}

} /* namespace platform */
