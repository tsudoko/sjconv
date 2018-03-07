#define UTFmax 4

typedef struct {
	int n, i;
	char buf[UTFmax];
} State;

typedef State * iconv_t;

size_t  iconv(iconv_t, char **restrict, size_t *restrict, char **restrict, size_t *restrict);
int     iconv_close(iconv_t);
iconv_t iconv_open(const char *, const char *);
