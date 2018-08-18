#include <stdbool.h>

#include <iconv.h>

#include "_iconv_wrapped.c"

/* iconv_wrapped.c - wrapper around system iconv(3) calls
 *
 * Use this if you want to add cp932 support to your existing iconv(3)
 * implementation.
 *
 * To be able to use this, your linker needs to support the --wrap option.
 * Example usage (gcc):
 *     gcc someprogramobj1.o someprogramobj2.o iconv_wrapped.o \
 *          -Wl,--wrap=iconv -Wl,--wrap=iconv_open -Wl,--wrap=iconv_close \
 *          -o someprogram
 */

size_t __real_iconv(iconv_t cd, char **restrict inbuf, size_t *restrict inleft, char **restrict outbuf, size_t *restrict outleft);
iconv_t __real_iconv_open(const char *to, const char *from);
int __real_iconv_close(iconv_t cd);

struct sjconv_state {
	union {
		struct sjc_sjconv_state *sjc;
		iconv_t real;
	} cd;
	bool is_sjconv;
};

size_t
__wrap_iconv(struct sjconv_state *s, char **restrict inbuf, size_t *restrict inleft, char **restrict outbuf, size_t *restrict outleft)
{
	if(s->is_sjconv)
		return sjc_iconv(s->cd.sjc, inbuf, inleft, outbuf, outleft);
	else
		return __real_iconv(s->cd.real, inbuf, inleft, outbuf, outleft);
}

iconv_t
__wrap_iconv_open(const char *to, const char *from)
{

	struct sjconv_state *s = malloc(sizeof *s);
	iconv_t cd = __real_iconv_open(to, from);
	if(cd != (iconv_t)-1) {
		s->is_sjconv = false;
		s->cd.real = cd;
		return s;
	}

	/* TODO: try converting to utf-8 and then to the target encoding for wider support? */
	struct sjc_sjconv_state *scd = sjc_iconv_open(to, from);
	if(scd != (struct sjc_sjconv_state *)-1) {
		s->is_sjconv = true;
		s->cd.sjc = scd;
		return s;
	}

	free(s);
	return (iconv_t)-1;
}

int
__wrap_iconv_close(struct sjconv_state *s)
{
	int ret;
	if(s->is_sjconv) {
		if((ret = sjc_iconv_close(s->cd.sjc)) != 0)
			return ret;
	} else {
		if((ret = __real_iconv_close(s->cd.real)) != 0)
			return ret;
	}

	free(s);
	return 0;
}
