/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 ** mu-stream.cc: library stream functions
 **
 **/
#include <sys/stat.h>

#include <cassert>
#include <fstream>
#include <iostream>

#include "libmu/compiler.h"
#include "libmu/env.h"
#include "libmu/eval.h"
#include "libmu/print.h"
#include "libmu/read.h"
#include "libmu/readtable.h"
#include "libmu/type.h"

#include "libmu/types/char.h"
#include "libmu/types/cons.h"
#include "libmu/types/exception.h"
#include "libmu/types/function.h"
#include "libmu/types/namespace.h"
#include "libmu/types/stream.h"

namespace libmu {
namespace mu {

using Frame = Env::Frame;

/** * mu function (stream? form) => bool **/
void IsStream(Frame* fp) {
  fp->value = Type::BoolOf(Stream::IsType(fp->argv[0]));
}

/** * mu function (eof? stream) **/
void IsEof(Frame* fp) {
  auto stream = Stream::StreamDesignator(fp->env, fp->argv[0]);

  if (!Stream::IsType(stream))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(eof?)",
                     fp->argv[0]);

  fp->value = Type::BoolOf(Stream::IsEof(stream));
}

/** * (read-char stream) => char **/
void ReadChar(Frame* fp) {
  auto stream = Stream::StreamDesignator(fp->env, fp->argv[0]);

  if (!Stream::IsType(stream))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "not an input stream designator (read-ch)", fp->argv[0]);

  if (Stream::IsEof(stream))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::END_OF_FILE, "(read-ch)",
                     fp->argv[0]);

  if (Stream::IsFunction(stream)) {
    auto ch = Function::Funcall(fp->env, Stream::func(stream), std::vector<TagPtr>{});

    if (!Char::IsType(ch))
      Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "function stream returns non-char (read-char)", stream);
    fp->value = ch;
  } else {
    auto byte = Stream::ReadByte(fp->env, stream);

    fp->value = Type::Null(byte) ? byte
                                 : Char(Fixnum::Uint64Of(byte)).tag_;
  }
}

/** * (unread-char ch stream) => char **/
void UnReadChar(Frame* fp) {
  auto ch = fp->argv[0];
  auto stream = Stream::StreamDesignator(fp->env, fp->argv[1]);

  if (!Char::IsType(ch))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "not a character (unread-char)", ch);

  if (!Stream::IsType(stream))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "not an input stream (unread-char)", stream);

  if (Stream::IsFunction(stream))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "may not reverse time on a function stream (unread-char)",
                     stream);

  fp->value =
    Stream::UnReadByte(Fixnum(Char::Uint8Of(fp->argv[0])).tag_, stream);
}

/** * (read-byte stream) => fixnum **/
void ReadByte(Frame* fp) {
  auto stream = Stream::StreamDesignator(fp->env, fp->argv[0]);

  if (!Stream::IsType(stream))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "not an input stream designator (read-ch)", fp->argv[0]);

  if (Stream::IsEof(stream))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::END_OF_FILE, "(read-ch)",
                     fp->argv[0]);

  if (Stream::IsFunction(stream)) {
    auto byte = Function::Funcall(fp->env, Stream::func(stream), std::vector<TagPtr>{});

    if (!Fixnum::IsType(byte) || Fixnum::Int64Of(byte) < 0 || Fixnum::Int64Of(byte) > 255)
      Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                       "function stream returns non-byte (read-byte)", stream);
    fp->value = byte;
  } else {
    fp->value = Stream::ReadByte(fp->env, stream);
  }
}

/** * (write-char char stream) => char  **/
void WriteChar(Frame* fp) {
  auto ch = fp->argv[0];
  auto stream = Stream::StreamDesignator(fp->env, fp->argv[1]);

  if (!Char::IsType(ch))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(write-char)",
                     ch);

  if (!Stream::IsType(stream))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "not an output stream designator (write-char)",
                     fp->argv[0]);

  Stream::WriteByte(Fixnum(Char::Uint8Of(ch)).tag_, stream);

  fp->value = ch;
}

/** * (write-byte fixnum stream) => fixnum  **/
void WriteByte(Frame* fp) {
  auto byte = fp->argv[0];
  auto stream = Stream::StreamDesignator(fp->env, fp->argv[1]);

  if (!Fixnum::IsType(byte))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "(write-byte)",
                     byte);

  if (!Stream::IsType(stream))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "not an output stream designator (write-byte)",
                     fp->argv[0]);

  Stream::WriteByte(byte, stream);

  fp->value = byte;
}

/** *  (input-file path) => stream **/
void InFileStream(Frame* fp) {
  auto pathname = fp->argv[0];

  if (!String::IsType(pathname))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "argument must be a filespec (input-file)", pathname);

  fp->value = Stream::MakeInputFile(fp->env, String::StdStringOf(pathname));
}

/** *  (make-input-string string) => stream **/
void InStringStream(Frame* fp) {
  auto in_string = fp->argv[0];

  if (!String::IsType(in_string))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "argument must be a string (make-input-string)",
                     in_string);

  fp->value = Stream::MakeInputString(fp->env, String::StdStringOf(in_string));
}

/** *  (make-output-string string) => stream **/
void OutStringStream(Frame* fp) {
  auto init_string = fp->argv[0];

  if (!String::IsType(init_string))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "argument must be a string (make-output-string)",
                     init_string);

  fp->value =
    Stream::MakeOutputString(fp->env, String::StdStringOf(init_string));
}

/** * (output-file path) => stream **/
void OutFileStream(Frame* fp) {
  auto path = fp->argv[0];

  if (!String::IsType(path))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "argument must be a filespec (make-output-file)", path);

  fp->value = Stream::MakeOutputFile(fp->env, String::StdStringOf(path));
}

/** * (get-output-string-stream stream) => string **/
void GetStringStream(Frame* fp) {
  auto stream = fp->argv[0];

  if (!Stream::IsType(stream))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "argument must be a stream (get-output-string-stream)",
                     stream);

  auto sp = Type::Untag<Stream::Layout>(stream);

  if (!Platform::IsString(sp->stream))
    Exception::Raise(
        fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
        "argument must be a string stream (get-output-string-stream)", stream);

  fp->value = String(fp->env, Platform::GetStdString(sp->stream)).tag_;
}

  /** * (open-socket-server port) **/
void SocketServerStream(Frame* fp) {
  auto port = fp->argv[0];

  if (!Fixnum::IsType(port))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "argument must be a fixnum (open-socket-server)",
                     port);

  fp->value = Type::NIL;
};

/** * (open-function-stream fn) **/
void FunctionStream(Frame* fp) {
  auto fn = fp->argv[0];

  if (!Function::IsType(fn))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "argument must be a function (open-function-stream)",
                     fn);

  fp->value = Stream(fn).Evict(fp->env, "mu-stream:function-stream");
};

/** * (accept-socket-stream stream) **/
void AcceptSocketStream(Frame* fp) {
  auto socket = fp->argv[0];

  if (!Stream::IsType(socket))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "argument must be a stream (accept-socket-stream)",
                     socket);

  fp->value = Type::NIL;
};

/** * (open-socket-stream ipaddr port) **/
void SocketStream(Frame* fp) {
  auto ipaddr = fp->argv[0];
  auto port = fp->argv[1];

  if (!Fixnum::IsType(ipaddr))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "argument must be a fixnum (make-socket-stream)",
                     ipaddr);

  if (!Fixnum::IsType(port))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "argument must be a fixnum (make-socket-stream)",
                     port);

  fp->value = Type::NIL;
};

/** * (connect-socket-stream stream) **/
void ConnectSocketStream(Frame* fp) {
  auto socket = fp->argv[0];

  if (!Stream::IsType(socket))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "argument must be a stream (make-socket-server)",
                     socket);

  fp->value = Type::NIL;
};

/** * (close stream) =>  **/
void Close(Frame* fp) {
  auto stream = fp->argv[0];

  if (!Stream::IsType(stream))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "argument must be a stream (close)", stream);

  Stream::Close(stream);
  fp->value = Type::T;
}

/** * (load path) => bool **/
void Load(Frame* fp) {
  auto filespec = fp->argv[0];

  if (!String::IsType(filespec))
    Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR,
                     "argument must be a filespec (load)", filespec);

  switch (Type::TypeOf(filespec)) {
    case SYS_CLASS::STREAM: {
      auto sp = Type::Untag<Stream::Layout>(filespec);
      while (!Platform::IsEof(sp->stream))
        Eval(fp->env, Compiler::Compile(fp->env, Read(fp->env, filespec)));

      break;
    }
    case SYS_CLASS::STRING: {
      auto istream =
          Stream::MakeInputFile(fp->env, String::StdStringOf(filespec));

      if (Type::Null(istream))
        Exception::Raise(fp->env, Exception::EXCEPT_CLASS::FILE_ERROR, "(load)",
                         filespec);

      auto sp = Type::Untag<Stream::Layout>(istream);

      while (!Platform::IsEof(sp->stream))
        Eval(fp->env, Compiler::Compile(fp->env, Read(fp->env, istream)));
      
      if (Type::Null(Stream::Close(istream)))
        Exception::Raise(fp->env, Exception::EXCEPT_CLASS::STREAM_ERROR,
                         "couldn't close (load)", filespec);

      break;
    }
    default:
      Exception::Raise(fp->env, Exception::EXCEPT_CLASS::TYPE_ERROR, "load",
                       filespec);
      break;
  }

  fp->value = Type::T;
}

} /* namespace mu */
} /* namespace libmu */
