#
# top-level makefile
#
MAKE = make

.PHONY: release fmt debug profile tests install clean

help:
	@echo "clean - remove binaries"
	@echo "debug - build debug"
	@echo "format - format source"
	@echo "install - install release"
	@echo "profile - build profile"
	@echo "release - build release"
	@echo "tests - run tests"

release:
	@$(MAKE) -C build release --no-print-directory

profile:
	@$(MAKE) -C build profile --no-print-directory

debug:
	@$(MAKE) -C build debug --no-print-directory

format:
	@$(MAKE) -C build format --no-print-directory

tests:
	@$(MAKE) -C tests --no-print-directory

install:
	@$(MAKE) -C dist uninstall install --no-print-directory

clean:
	@$(MAKE) -C build clean --no-print-directory

