#
# libmu.so makefile
#
.PHONY: clean libmu.so

libmu.so: libmu.a
	@$(AR) x libmu.a
	@$(CXX) -shared *.o -o libmu.so
	@rm -f *.o *__.SYMDEF*

clean:
	@rm -rf libmu.so *.o *__SYMDEF*
