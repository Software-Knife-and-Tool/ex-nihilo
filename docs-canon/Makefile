#
# documents
#
.PHONY: libmu xref
all: libmu xref

libmu:
	@make -C libmu

xref:
	tagxref.pl ../src/libmu | sort -g > libmu.xref
