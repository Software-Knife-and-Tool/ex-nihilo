#
# posix install makefile
#
BUILD = ../build
CANON = ../src/canon/canon
CORE = ../src/canon/core
DEST = /usr/local/logica
LIBMU = ../src/libmu
PLATFORM = ../src/platform

.PHONY: install uninstall help

help:
	@echo install - install logica in /usr/local

install:
	@install -d $(DEST)
	@install -d $(DEST)/bin
	@install -d $(DEST)/canon/canon
	@install -d $(DEST)/canon/libmu
	@install -d $(DEST)/canon/core
	@install -d $(DEST)/canon/platform
	@install -m 644 $(BUILD)/mu.a $(DEST)/canon/libmu
	@install -m 644 $(BUILD)/libmu.so $(DEST)/canon/libmu
	@install -m 644 $(BUILD)/platform.a $(DEST)/canon/platform
	@install -m 755 $(BUILD)/mu-repl $(DEST)/bin
	@install -m 755 $(BUILD)/mu-exec $(DEST)/bin
#	@install -m 755 $(BUILD)/mu-load $(DEST)/bin
	@install -m 755 ./logica.sh $(DEST)/bin/logica
	@install -m 755 ./canon.sh $(DEST)/bin/canon
	@install -m 644 $(CANON)/*.l $(DEST)/canon/canon
	@install -m 644 $(LIBMU)/libmu.h $(DEST)/canon/libmu
	@install -m 644 $(PLATFORM)/platform.h $(DEST)/canon/platform
	@install -m 644 $(CORE)/* $(DEST)/canon/core

uninstall:
	@rm -rf $(DEST)
