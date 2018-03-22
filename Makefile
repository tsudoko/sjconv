.PHONY: all clean

#TARG = iconv.a
OFILES = iconv.o

all: $(OFILES)
iconv.c: _cp932tab.c
_cp932tab.c: cp932tab.awk CP932.TXT
	awk -f cp932tab.awk -F'	' CP932.TXT > $@

clean:
	rm -f $(TARG) $(OFILES) _cp932tab.c
