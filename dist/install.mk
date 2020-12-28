#
# posix install makefile
#
ROOT = /opt
BASE = gyre

.PHONY: install uninstall help

help:
	@echo install - install gyre in $(ROOT)/$(BASE) (needs sudo)
	@echo uninstall - remove gyre from $(ROOT)/$(BASE) (needs sudo)

install:
	@cat $(BASE)-0.0.1.tgz | (cd $(ROOT); tar xfz -)

uninstall:
	@rm -rf $(ROOT)/$(BASE)
