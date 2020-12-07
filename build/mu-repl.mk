#
# mu-repl makefile
#
CXX      = $(CXX)
AR       = $(AR)
CXXFLAGS = $(CXXFLAGS)
CPPFLAGS = -I ../src

OBJS =  \
    main.o      \
    opts.o      \
    repl.o

vpath %.cc ../src/mu-repl

LIBS = ./mu.a ./platform.a

# .SILENT: $(OBJS)
.PHONY: mu-repl clean

mu-repl: $(OBJS)
	@$(CXX) -o mu-repl $(CPPFLAGS) $(CXXFLAGS) $(OBJS) $(LIBS)
	@rm $(OBJS)

clean:
	@rm -f $(OBJS) mu-repl
