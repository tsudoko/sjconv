#include <stdlib.h>
#include <string.h>

#define _POSIX_SOURCE
#include <errno.h>

#include "iconv.h"

/* iconv.c - iconv-compatible SJIS→UTF-8 converter */
/* doesn't handle incomplete multibyte sequences in the input buffer */

typedef unsigned long Rune;
#include "sjistab.c"

enum {
	EncSJIS,
	EncCP932,
	EncUTF8,
	EncInvalid = -1,
};

char *enctab[4][4] = {
	[EncSJIS]  = {"SHIFT_JIS", "SJIS", "SHIFT-JIS", NULL},
	[EncCP932] = {"CP932", "WINDOWS-31J", NULL},
	[EncUTF8]  = {"UTF-8", "UTF8", NULL},
	NULL,
};

int
encget(const char *name)
{
	for(int i = 0; enctab[i] != NULL; i++)
		for(int j = 0; enctab[i][j] != NULL; j++)
			if(strcmp(enctab[i][j], name) == 0)  /* TODO: lowercase names? */
				return i;

	return EncInvalid;
}

static int
sjistorune(Rune *r, unsigned char *s)
{
	Rune buf;
	int ret = 0;

	/* printf("sjistorune: *s 0x%x\n", *s); */
	if((*s >= 0x81 && *s <= 0x9f) || (*s >= 0xe0 && *s <= 0xef)) {
		buf = *(s++)<<8;
		buf |= *s;
		ret = 2;
	} else if(*s < 0x20 || *s == 0x7f) { /* non-printable ascii not present in SHIFTJIS.TXT */
		*r = *s;
		return 1;
	} else {
		buf = *s;
		ret = 1;
	}

	*r = sjistab[buf];
	return ret;
}

static int
runetochar(char *s, Rune *r)
{
	if(*r == (*r&0x7f)) {
		*s = *r&0x7f;
		return 1;
	} else if(*r == (*r&0x7ff)) {
		*(s++) = 0300 | ((*r>>6)&0277);
		*s = 0200 | (*r&077);
		return 2;
	} else if(*r == (*r&0xffff)) {
		*(s++) = 0340 | ((*r>>12)&0337);
		*(s++) = 0200 | ((*r>>6)&077);
		*s = 0200 | (*r&077);
		return 3;
	} else if(*r == (*r&0x10ffff)) {
		*(s++) = 0360 | ((*r>>18)&0357);
		*(s++) = 0200 | ((*r>>12)&077);
		*(s++) = 0200 | ((*r>>6)&077);
		*s = 0200 | (*r&077);
		return 4;
	} else {
		return -1;
	}
}

size_t
iconv(iconv_t cd, char **restrict inbuf, size_t *restrict inleft, char **restrict outbuf, size_t *restrict outleft)
{
	State *s = (State *)cd;
	Rune r;

	if(s->n) {
		if(*outleft >= s->n) {
			memcpy(*outbuf, s->buf+s->i, s->n);
			*outbuf += s->n;
			*outleft -= s->n;
			s->n = 0;
		} else {
			memcpy(*outbuf, s->buf+s->i, *outleft);
			*outbuf += *outleft;
			*outleft -= *outleft;
			s->i = *outleft;
			s->n = s->n-*outleft;
			errno = E2BIG;
			return (size_t)-1;
		}
	}

	while(*inleft) {
		int n = sjistorune(&r, (unsigned char *)*inbuf);
		*inbuf += n;
		*inleft -= n;
		n = runetochar(s->buf, &r);
		if(*outleft >= n) {
			memcpy(*outbuf, s->buf, n);
			*outbuf += n;
			*outleft -= n;
		} else {
			memcpy(*outbuf, s->buf, *outleft);
			*outbuf += *outleft;
			*outleft -= *outleft;
			s->i = *outleft;
			s->n = n-*outleft;
			errno = E2BIG;
			return (size_t)-1;
		}
	}

	return (size_t)0;
}

iconv_t
iconv_open(const char *to, const char *from)
{
	int tocode = encget(to), fromcode = encget(from);
	iconv_t cd;

	if(tocode != EncUTF8 || fromcode != EncSJIS) {
		errno = EINVAL;
		return (iconv_t)-1;
	}

	cd = malloc(sizeof *cd);
	if(cd == NULL)
		return (iconv_t)-1;
	cd->n = 0;
	cd->i = 0;

	return cd;
}

int
iconv_close(iconv_t cd)
{
	if(cd == NULL) {
		errno = EBADF;
		return -1;
	}

	free(cd);
	return 0;
}
