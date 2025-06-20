#include "string_utils.h"

void print_mg_str(struct mg_str s) {
    if (s.len == 0 || s.buf == NULL) {
        printf("(null)\n");
        return;
    }
    printf("%.*s\n", (int) s.len, s.buf);
}


char *mg_str_to_cstr(const struct mg_str *s) {
    char *res = (char *) malloc(s->len + 1);
    if (res == NULL) return NULL;
    memcpy(res, s->buf, s->len);
    res[s->len] = '\0';
    return res;
}