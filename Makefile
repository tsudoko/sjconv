.PHONY: all clean

#TARG = iconv.a
OFILES = iconv.o

all: $(OFILES)
src/iconv.c: src/sjistab.c
src/sjistab.c: res/SHIFTJIS.TXT
	awk -f res/sjistabgen.awk $< > $@

clean:
	rm -f $(TARG) $(OFILES) src/sjistab.c
