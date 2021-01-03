#
# posix platform makefile
#
MAKE        = make
CXX         = clang++ 
AR          = ar
DCXXFLAGS   = -m64 -Wall -Wpedantic -Wextra -std=c++14 -g -fPIC
PCXXFLAGS   = -m64 -Wall -Wpedantic -wextra -std=c++14 -fprofile-instr-generate -fcoverage-mapping -fPIC
RCXXFLAGS   = -m64 -Wall -Wpedantic -Wextra -std=c++14 -O2 -fPIC 

DMAKEFLAGS = CXX="$(CXX)" AR="$(AR)" CXXFLAGS="$(DCXXFLAGS)"
PMAKEFLAGS = CXX="$(CXX)" AR="$(AR)" CXXFLAGS="$(PCXXFLAGS)"
RMAKEFLAGS = CXX="$(CXX)" AR="$(AR)" CXXFLAGS="$(RCXXFLAGS)"

MAKES = libmu.mk libmu.so.mk mu-exec.mk

.PHONY: commit install format xref release debug profile clean tidy

help:
	@echo make clean - clean executables and libraries
	@echo make debug - build for debugging
	@echo make format - format source
	@echo make profile - build for profiling
	@echo make release - build for release
	@echo make tidy - run clang-tidy

release: clean
	@for i in $(MAKES); do		\
            make -f $$i $(RMAKEFLAGS);	\
	done

debug: clean
	@for i in $(MAKES); do		\
	    make -f $$i $(DMAKEFLAGS);	\
	done

profile: clean
	@for i in $(MAKES); do		\
            make -f $$i $(PMAKEFLAGS);	\
	done

clean:
	@for i in $(MAKES); do		\
	    $(MAKE) -f $$i clean;	\
	done
	@rm -rf googletest

tidy:
	@for f in ../src/libmu/*.cc ../src/libmu/heap/*.cc ../src/libmu/platform/*.cc ../src/libmu/types/*.cc ../src/libmu/mu/*.cc; do	\
	    clang-tidy 									\
	        -checks='cppcoreguidelines*,clang-diagnostic-*,clang-analyzer-*,-*,bugprone*,modernize*,performance*,-modernize-pass-by-value,-modernize-use-auto,-modernize-use-using'	\
		$$f -- 									\
		-I../src								\
		-I/Library/Developer/CommandLineTools/usr/include/c++/v1		\
		-I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include	\
		-I/Library/Developer/CommandLineTools/usr/lib/clang/10.0.1/include;	\
	done
format:
	@(cd ../src/libmu && clang-format -i *.h *.cc */*.h */*.cc)
	@(cd ../src/mu-exec && clang-format -i *.cc)

