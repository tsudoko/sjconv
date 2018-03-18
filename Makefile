.PHONY: all clean

#TARG = iconv.a
OFILES = iconv.o

src/sjistab.c: gen/sjistab.awk res/SHIFTJIS.TXT
	awk -f gen/sjistab.awk res/SHIFTJIS.TXT > $@

clean:
	rm -f $(TARG) $(OFILES) src/sjistab.c
