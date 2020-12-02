#
# mu.a makefile
#
CXX      = $(CXX)
AR       = $(AR)
CXXFLAGS = $(CXXFLAGS)
CPPFLAGS = -I ../src

OBJS =  \
    char.o             \
    compiler.o         \
    cons.o             \
    env.o              \
    eval.o             \
    exception.o        \
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
    special.o          \
    stream.o           \
    string.o           \
    symbol.o           \
    type.o             \
    vector-types.o     \
    vector.o           \
    mu-char.o          \
    mu-cons.o          \
    mu-eval.o          \
    mu-exception.o     \
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
    mu-vector.o  

vpath %.cc ../src/libmu
vpath %.cc ../src/libmu/heap
vpath %.cc ../src/libmu/mu
vpath %.cc ../src/libmu/special
vpath %.cc ../src/libmu/types

# .SILENT: $(OBJS)
.PHONY: release debug profile clean mu.a

libmu.a: $(OBJS)
	@$(AR) cr mu.a $(OBJS)
	@rm -f $(OBJS)

release debug profile: mu.a

clean:
	@rm -rf mu.a $(OBJS)
