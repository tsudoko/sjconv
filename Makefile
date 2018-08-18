.PHONY: all clean

#TARG = iconv.a
OFILES = iconv.o

# uncomment if you want to use iconv_wrapped
#OFILES += iconv_wrapped.o

all: $(OFILES)
iconv.c: _cp932tab.c
_cp932tab.c: cp932tab.awk CP932.TXT
	awk -f cp932tab.awk -F'	' CP932.TXT > $@

iconv_wrapped.c: _iconv_wrapped.c
_iconv_wrapped.c: iconv.c
	sed -e 's/^#include "iconv.h"//' \
		-e 's/iconv_t/struct sjconv_state */g' \
		-e 's/^iconv/sjc_&/' \
		-e 's/struct sjconv_state/struct sjc_sjconv_state/' $< > $@

clean:
	rm -f $(TARG) $(OFILES) _cp932tab.c _iconv_wrapped.c
