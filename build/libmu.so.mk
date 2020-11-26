#
# mu.so makefile
#
.PHONY: clean libmu.so

libmu.so: mu.a platform.a
	@$(AR) x platform.a
	@for i in *.o; do 		\
	    mv $$i platform-$$i;	\
	done
	@$(AR) x mu.a
	@$(CXX) -shared *.o -o libmu.so
	@rm -f *.o *__.SYMDEF*

clean:
	@rm -rf libmu.so *.o *__SYMDEF*
