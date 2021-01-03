/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 ** stream.cc: platform streams
 **
 **/
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <cstdlib>
#include <cstring>

#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>

#include "libmu/platform/platform-stream.h"
#include "libmu/platform/platform.h"

namespace libmu {
namespace platform {

auto Platform::IsClosed(StreamId stream) -> bool {
  auto sp = StructOfStreamId(stream);

  return (sp->flags & STREAM_CLOSED) ? true : false;
}

auto Platform::IsOutput(StreamId stream) -> bool {
  auto sp = StructOfStreamId(stream);

  return (sp->flags & STREAM_OUTPUT) ? true : false;
}

auto Platform::IsString(StreamId stream) -> bool {
  auto sp = StructOfStreamId(stream);

  return (sp->flags & STREAM_STRING) ? true : false;
}

auto Platform::IsEof(StreamId stream) -> bool {
  auto sp = StructOfStreamId(stream);

  if (sp->flags & STREAM_STD) {
    switch (sp->u.stdstream) {
      case STDIN:
        return feof(stdin);
        break;
      default:
        assert(false);
        return -1;
        break;
    }
  }

  return sp->u.istream->eof();
}

auto Platform::GetStdString(StreamId stream) -> std::string {
  auto sp = StructOfStreamId(stream);
  auto str = sp->u.sstream->str();

  sp->u.sstream->str(std::string());
  return str;
}

auto Platform::Flush(StreamId stream) -> void {
  auto sp = StructOfStreamId(stream);

  if ((sp->flags & STREAM_STD) && (sp->flags & STREAM_OUTPUT)) fflush(stdout);
}

auto Platform::WriteByte(int ch, Platform::StreamId stream) -> void {
  auto sp = StructOfStreamId(stream);

  assert(sp->flags & STREAM_OUTPUT);

  if ((sp->flags & STREAM_CLOSED) == 0) {
    if (sp->flags & STREAM_STRING) {
      sp->u.sstream->str(sp->u.sstream->str() + (char)ch);
    } else if (sp->flags & STREAM_STD) {
      switch (sp->u.stdstream) {
        case STDOUT:
          putc(ch, stdout);
          break;
        case STDERR:
          putc(ch, stderr);
          break;
        case STDIN:
          assert(false);
          break;
      }
    } else if (sp->flags & STREAM_IOS) {
      sp->u.ostream->put(ch);
    } else {
      assert(false);
    }
  }
}

auto Platform::ReadByte(Platform::StreamId stream) -> int {
  auto sp = StructOfStreamId(stream);
  int ch;

  assert(sp->flags & STREAM_INPUT || sp->flags & STREAM_STRING);

  if (sp->flags & STREAM_CLOSED) return -1;

  if (sp->flags & STREAM_STRING) {
    ch = sp->u.sstream->get();
    if (sp->u.sstream->eof()) {
      return -1;
    }
  } else if (sp->flags & STREAM_STD) {
    switch (sp->u.stdstream) {
      case STDIN:
        ch = getc(stdin);
        break;
      default:
        assert(false);
        return -1;
        break;
    }
  } else if (sp->flags & STREAM_IOS) {
    ch = sp->u.istream->get();
    if (sp->u.istream->eof()) {
      return -1;
    }
  } else {
    assert(false);
  }

  if (ch == 0x4) return -1;

  return ch;
}

auto Platform::UnReadByte(int ch, Platform::StreamId stream) -> int {
  auto sp = StructOfStreamId(stream);

  assert(sp->flags & STREAM_INPUT || sp->flags & STREAM_STRING);

  if (sp->flags & STREAM_CLOSED) {
    return -1;
  }

  if (sp->flags & STREAM_STRING) {
    sp->u.sstream->putback(ch);
  } else if (sp->flags & STREAM_STD) {
    switch (sp->u.stdstream) {
      case STDIN:
        return ungetc(ch, stdin);
        break;
      default:
        assert(false);
        return -1;
        break;
    }
  } else if (sp->flags & STREAM_IOS) {
    sp->u.istream->putback(ch);
  } else
    assert(false);

  return ch;
}

auto Platform::OpenInputFile(const std::string &pathname)
    -> Platform::StreamId {
  struct stat fileInfo;

  if (stat(pathname.c_str(), &fileInfo) == -1) {
    return STREAM_ERROR;
  }

  std::ifstream *ifs = new std::ifstream;

  ifs->open(pathname, std::fstream::in);
  if ((ifs->rdstate() & std::ifstream::failbit) != 0) {
    return STREAM_ERROR;
  }

  auto sp = new Stream();
  sp->flags = STREAM_IOS | STREAM_INPUT;
  sp->u.istream = ifs;

  return StreamIdOf(sp);
}

auto Platform::OpenOutputFile(const std::string &pathname)
    -> Platform::StreamId {
  std::ofstream *ofs = new std::ofstream;

  ofs->open(pathname, std::fstream::out);
  if ((ofs->rdstate() & std::ofstream::failbit) != 0) {
    return STREAM_ERROR;
  }

  auto sp = new Stream();
  sp->flags = STREAM_IOS | STREAM_OUTPUT;
  sp->u.ostream = ofs;

  return StreamIdOf(sp);
}

auto Platform::OpenOutputString(const std::string &init) -> Platform::StreamId {
  std::stringstream *ofs = new std::stringstream(init);

  auto sp = new Stream();
  sp->flags = STREAM_STRING | STREAM_OUTPUT;
  sp->u.sstream = ofs;

  return StreamIdOf(sp);
}

auto Platform::OpenInputString(const std::string &str) -> Platform::StreamId {
  std::stringstream *ifs = new std::stringstream(str);

  auto sp = new Stream();
  sp->flags = STREAM_STRING | STREAM_INPUT;
  sp->u.sstream = ifs;

  return StreamIdOf(sp);
}

auto Platform::OpenStandardStream(Platform::STD_STREAM stream)
    -> Platform::StreamId {
  auto sp = new Stream();

  switch (stream) {
    case Platform::STD_STREAM::STDIN:
      sp->flags = STREAM_STD | STREAM_INPUT;
      sp->u.stdstream = stream;
      break;
    case Platform::STD_STREAM::STDOUT: /* fall through */
    case Platform::STD_STREAM::STDERR:
      sp->flags = STREAM_STD | STREAM_OUTPUT;
      sp->u.stdstream = stream;
      break;
  }

  return StreamIdOf(sp);
}

auto Platform::OpenServerSocketStream(int port) -> Platform::StreamId {
  struct sockaddr_in address;
  int opt = 1;
  auto socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_fd < 0) {
    return STREAM_ERROR;
  }

  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    return STREAM_ERROR;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    return STREAM_ERROR;
  }

  auto sp = new Stream();
  sp->u.sostream = socket_fd;

  return StreamIdOf(sp);
}

auto Platform::AcceptSocketStream(Platform::StreamId) -> Platform::StreamId {
  auto socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_fd < 0) {
    return STREAM_ERROR;
  }

#if 0
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    valread = read( new_socket , buffer, 1024);
    printf("%s\n",buffer );
    send(new_socket , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
    return 0;
#endif

  auto sp = new Stream();
  sp->u.sostream = socket_fd;

  return StreamIdOf(sp);
}

auto Platform::OpenClientSocketStream(int ipaddr, int port)
    -> Platform::StreamId {
  auto sp = new Stream();
  (void)ipaddr;
  (void)port;

  return StreamIdOf(sp);
}

auto Platform::ConnectSocketStream(Platform::StreamId) -> Platform::StreamId {
  auto socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_fd < 0) return STREAM_ERROR;

#if 0
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return STREAM_ERROR;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/Address not supported \n");
        return STREAM_ERROR;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return STREAM_ERROR;
    }
    send(sock , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
    valread = read( sock , buffer, 1024);
    printf("%s\n",buffer );
#endif

  auto sp = new Stream();
  sp->u.sostream = socket_fd;

  return StreamIdOf(sp);
}

auto Platform::Close(StreamId stream) -> void {
  auto sp = StructOfStreamId(stream);

  if (sp->flags & STREAM_STD) {
    return;
  }

  if (sp->flags & STREAM_INPUT) {
    delete sp->u.istream;
  }

  if (sp->flags & STREAM_OUTPUT) {
    sp->u.ostream->flush();
    delete sp->u.ostream;
  }

  sp->flags |= STREAM_CLOSED;
}

} /* namespace platform */
} /* namespace libmu */
