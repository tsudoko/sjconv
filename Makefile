.PHONY: all clean

#TARG = iconv.a
OFILES = iconv.o

all: $(OFILES)
iconv.c: _sjistab.c
_sjistab.c: sjistab.awk SHIFTJIS.TXT
	awk -f sjistab.awk SHIFTJIS.TXT > $@

clean:
	rm -f $(TARG) $(OFILES) _sjistab.c
