#
# posix release makefile
#
BUILD = ../build
GYRE = ../src/gyre
CORE = ../src/core
DEST = gyre
LIBMU = ../src/libmu

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
	@install -m 755 ../dist/core.sh $(DEST)/bin/core
	@install -m 755 ../dist/gyre.sh $(DEST)/bin/gyre
	@install -m 755 ../dist/mu.sh $(DEST)/bin/mu
	@install -m 755 $(BUILD)/mu-repl $(DEST)/bin
	@install -m 755 $(BUILD)/mu-exec $(DEST)/bin
	@install -m 644 $(BUILD)/libmu.a $(DEST)/lib
	@install -m 644 $(BUILD)/libmu.so $(DEST)/lib
	@install -m 644 $(GYRE)/*.l $(DEST)/src/gyre
	@install -m 644 $(CORE)/*.l $(DEST)/src/core
	@install -m 644 $(LIBMU)/libmu.h $(DEST)/include/libmu
	@install -m 644 $(LIBMU)/platform/platform.h $(DEST)/include/libmu/platform
	@tar cvfz $(DEST)-0.0.1.tgz $(DEST)
	@rm -rf $(DEST)

clean:
	@rm -rf $(DEST) $(DEST)-0.0.1.tgz
