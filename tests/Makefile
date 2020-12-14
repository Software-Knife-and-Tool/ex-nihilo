#
# tests makefile
#
.PHONY: tests

tests:
	@./run-tests libmu
	@./run-tests mu "-l ../src/core/mu.l"
	@./run-tests core "-l ../src/core/mu.l" "-l ../src/core/core.l"
