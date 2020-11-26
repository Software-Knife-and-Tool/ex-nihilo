#
# platform.a makefile
#
CXX      = $(CXX)
AR       = $(AR)
CXXFLAGS = $(CXXFLAGS)
CPPFLAGS = -I ../src

OBJS =  \
    ffi.o       \
    platform.o	\
    stream.o    \
    task.o

vpath %.cc ../src/platform

# .SILENT: $(OBJS)
.PHONY: release debug profile clean unit-tests

release debug profile: platform.a

platform.a: $(OBJS)
	@$(AR) cr platform.a $(OBJS)
	@rm -f $(OBJS)

clean:
	@rm -f $(OBJS) platform.a
