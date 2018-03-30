#define UTFmax 4

struct sjconv_state;
typedef struct sjconv_state *iconv_t;

size_t  iconv(iconv_t, char **restrict, size_t *restrict, char **restrict, size_t *restrict);
int     iconv_close(iconv_t);
iconv_t iconv_open(const char *, const char *);
