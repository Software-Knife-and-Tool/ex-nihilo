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
    repl.o

vpath %.cc ../src/mu-exec

LIBS = ./libmu.a

.SILENT: $(OBJS)
.PHONY: mu-exec clean

mu-exec: $(OBJS)
	@$(CXX) -o mu-exec $(CPPFLAGS) $(CXXFLAGS) $(OBJS) $(LIBS)
	@rm $(OBJS)

clean:
	@rm -f $(OBJS) mu-exec
