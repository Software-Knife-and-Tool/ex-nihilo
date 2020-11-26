#
# mu-exec makefile
#
CXX      = $(CXX)
AR       = $(AR)
CXXFLAGS = $(CXXFLAGS)
CPPFLAGS = -I ../src

OBJS =  \
    main.o      \
    opts.o      \
    exec.o

vpath %.cc ../src/mu-exec

LIBS = ./mu.a ./platform.a

# .SILENT: $(OBJS)
.PHONY: mu-exec clean

mu-exec: $(OBJS)
	echo $(CXX) -o mu-exec $(CPPFLAGS) $(CXXFLAGS) $(OBJS) $(LIBS)
	@$(CXX) -o mu-exec $(CPPFLAGS) $(CXXFLAGS) $(OBJS) $(LIBS)
	@rm $(OBJS)

clean:
	@rm -f $(OBJS) mu-exec
