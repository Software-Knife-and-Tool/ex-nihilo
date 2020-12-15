#
# performance metrics makefile
#
.PHONY: all clean release base diff
TMP = /var/tmp

help:
	@echo make base - release tests
	@echo make release - release tests
	@echo make clean - clean intermediate files
	@echo make tests - run tests

release:
	@rm -f $(TMP)/base.$$PPID.log
	@for i in {0..2499}; do					\
           canon -l perf.l 					\
		 -l core.l					\
		 -l gc.l					\
		 -l map.l 					\
		 -q "(exit 0)" >> $(TMP)/base.$$PPID.log;	\
	done
	@canon -l perf.l -q "(perf-report \"$(TMP)/base.$$PPID.log\")" -q "(exit 0)" > release.perf
	@rm -f $(TMP)/base.$$PPID.log

base:
	@rm -f $(TMP)/base.$$PPID.log
	@for i in {0..2499}; do					\
           canon -l perf.l 					\
		 -l core.l					\
		 -l gc.l					\
		 -l map.l 					\
		 -q "(exit 0)" >> $(TMP)/base.$$PPID.log;	\
	done
	@canon -l perf.l -q "(perf-report \"$(TMP)/base.$$PPID.log\")" -q "(exit 0)" > base.perf
	@rm -f $(TMP)/base.$$PPID.log

diff:
	@paste base.perf release.perf

clean:
	@rm -f base.perf
