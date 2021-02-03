#
# top-level makefile
#
MAKE = make

.PHONY: release format debug dist profile tests install
.PHONY: uninstall clean perf-base perf perf-diff commit

help:
	@echo "---- build options"
	@echo "debug - build debug"
	@echo "profile - build profile"
	@echo "release - build release"
	@echo "clean - remove binaries"
	@echo "---- release"
	@echo "dist - build distribution for release"
	@echo "install - install release (needs sudo)"
	@echo "uninstall - uninstall release (needs sudo)"
	@echo "commit - run all pre-commit functions"
	@echo "--- pre-commit"
	@echo "perf-base - base perf measurement"
	@echo "perf - current perf measurement"
	@echo "perf-diff - display difference between base and current"
	@echo "format - format source"

commit: perf-base format
	@cat perf/base.perf

release:
	@$(MAKE) -C build release --no-print-directory

profile:
	@$(MAKE) -C build profile --no-print-directory

debug:
	@$(MAKE) -C build debug --no-print-directory

dist:
	@$(MAKE) -C dist -f release.mk release --no-print-directory

format:
	@$(MAKE) -C build format --no-print-directory

perf-base:
	@$(MAKE) -C perf base --no-print-directory
perf:
	@$(MAKE) -C perf release --no-print-directory
perf-diff:
	@$(MAKE) -C perf diff --no-print-directory

tests:
	@$(MAKE) -C tests --no-print-directory

install:
	@$(MAKE) -C dist -f install.mk uninstall install --no-print-directory

uninstall:
	@$(MAKE) -C dist -f install.mk uninstall --no-print-directory

clean:
	@$(MAKE) -C build clean --no-print-directory
	@$(MAKE) -C dist -f release.mk clean --no-print-directory
