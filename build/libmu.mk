#
# libmu.a makefile
#
CXX      = $(CXX)
AR       = $(AR)
CXXFLAGS = $(CXXFLAGS)
CPPFLAGS = -I ../src

OBJS =  \
    char.o             \
    compiler.o         \
    condition.o        \
    cons.o             \
    env.o              \
    eval.o             \
    fixnum.o           \
    float.o            \
    function.o         \
    heap.o             \
    libmu.o            \
    macro.o            \
    namespace.o        \
    print.o            \
    read.o             \
    readtable.o        \
    stream.o           \
    string.o           \
    symbol.o           \
    type.o             \
    vector-types.o     \
    vector.o           \
    mu-char.o          \
    mu-cons.o          \
    mu-env.o           \
    mu-condition.o     \
    mu-fixnum.o        \
    mu-float.o         \
    mu-function.o      \
    mu-heap.o          \
    mu-macro.o         \
    mu-namespace.o     \
    mu-platform.o      \
    mu-printer.o       \
    mu-reader.o        \
    mu-stream.o        \
    mu-struct.o        \
    mu-symbol.o        \
    mu-type.o          \
    mu-vector.o        \
    platform-ffi.o     \
    platform-stream.o  \
    platform-task.o    \
    platform.o	       \

vpath %.cc ../src/libmu
vpath %.cc ../src/libmu/heap
vpath %.cc ../src/libmu/mu
vpath %.cc ../src/libmu/platform
vpath %.cc ../src/libmu/special
vpath %.cc ../src/libmu/types

.SILENT: $(OBJS)
.PHONY: release debug profile clean libmu.a

libmu.a: $(OBJS)
	@$(AR) cr libmu.a $(OBJS)
	@rm -f $(OBJS)

release debug profile: libmu.a

clean:
	@rm -rf libmu.a $(OBJS)
