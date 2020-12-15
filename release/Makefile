#
# posix release makefile
#
BUILD = ../build
CANON = ../src/canon/canon
CORE = ../src/canon/core
DEST = ./logica
LIBMU = ../src/libmu
PLATFORM = ../src/platform

.PHONY: release clean help

help:
	@echo release
	@echo clean

release:
	@install -d $(DEST)
	@install -d $(DEST)/bin
	@install -d $(DEST)/canon/canon
	@install -d $(DEST)/canon/libmu
	@install -d $(DEST)/canon/core
	@install -d $(DEST)/canon/platform
	@install -m 644 $(BUILD)/libmu.a $(DEST)/canon/libmu
	@install -m 644 $(BUILD)/libmu.so $(DEST)/canon/libmu
	@install -m 644 $(BUILD)/libplatform.a $(DEST)/canon/platform
	@install -m 644 $(BUILD)/libplatform.so $(DEST)/canon/platform
	@install -m 755 $(BUILD)/mu-repl $(DEST)/bin
	@install -m 755 $(BUILD)/mu-exec $(DEST)/bin
#	@install -m 755 $(BUILD)/mu-load $(DEST)/bin
	@install -m 755 ../dist/logica.sh $(DEST)/bin/logica
	@install -m 755 ../dist/canon.sh $(DEST)/bin/canon
	@install -m 644 $(CANON)/*.l $(DEST)/canon/canon
	@install -m 644 $(LIBMU)/libmu.h $(DEST)/canon/libmu
	@install -m 644 $(PLATFORM)/platform.h $(DEST)/canon/platform
	@install -m 644 $(CORE)/* $(DEST)/canon/core
	@tar cvfz logica.tgz logica
	@rm -rf logica

clean:
	@rm -rf logica logica.tgz
