#
# posix release makefile
#
BUILD = ./
COMMON = ../src/common
COMPILER = ../src/compiler
CORE = ../src/core
DEST = gyre
GYRE = ../src/gyre
LIBMU = ../src/libmu
SVG = ../src/svg

.PHONY: release clean help

help:
	@echo release - package release
	@echo clean

release:
	@install -d $(DEST)
	@install -d $(DEST)/bin
	@install -d $(DEST)/include
	@install -d $(DEST)/include/libmu
	@install -d $(DEST)/include/libmu/platform
	@install -d $(DEST)/lib
	@install -d $(DEST)/src
	@install -d $(DEST)/src/gyre
	@install -d $(DEST)/src/core
	@install -d $(DEST)/src/common
	@install -d $(DEST)/src/compiler
	@install -d $(DEST)/src/compiler/asm
	@install -d $(DEST)/src/compiler/ast
	@install -d $(DEST)/src/compiler/emit
	@install -d $(DEST)/src/compiler/form
	@install -d $(DEST)/src/compiler/opt
	@install -d $(DEST)/src/compiler/reg-model
	@install -d $(DEST)/src/svg
	@install -m 755 ../dist/common.sh $(DEST)/bin/common
	@install -m 755 ../dist/core.sh $(DEST)/bin/core
	@install -m 755 ../dist/gyre.sh $(DEST)/bin/gyre
	@install -m 755 ../dist/mu.sh $(DEST)/bin/mu
	@install -m 755 $(BUILD)/mu-exec $(DEST)/bin
	@install -m 644 $(BUILD)/libmu.a $(DEST)/lib
	@install -m 644 $(BUILD)/libmu.so $(DEST)/lib
	@install -m 644 $(GYRE)/*.l $(DEST)/src/gyre
	@install -m 644 $(SVG)/*.l $(DEST)/src/svg
	@install -m 644 $(CORE)/*.l $(DEST)/src/core
	@install -m 644 $(COMMON)/*.l $(DEST)/src/common
	@install -m 644 $(COMPILER)/*.l $(DEST)/src/compiler
	@install -m 644 $(COMPILER)/asm/*.l $(DEST)/src/compiler/asm
	@install -m 644 $(COMPILER)/ast/*.l $(DEST)/src/compiler/ast
	@install -m 644 $(COMPILER)/emit/*.l $(DEST)/src/compiler/emit
	@install -m 644 $(COMPILER)/form/*.l $(DEST)/src/compiler/form
	@install -m 644 $(COMPILER)/opt/*.l $(DEST)/src/compiler/opt
	@install -m 644 $(COMPILER)/reg-model/*.l $(DEST)/src/compiler/reg-model
	@install -m 644 $(LIBMU)/libmu.h $(DEST)/include/libmu
	@install -m 644 $(LIBMU)/platform/platform.h $(DEST)/include/libmu/platform
	@tar cfz $(DEST)-0.0.1.tgz $(DEST)
	@rm -rf $(DEST)

clean:
	@rm -rf $(DEST) $(DEST)-0.0.1.tgz
