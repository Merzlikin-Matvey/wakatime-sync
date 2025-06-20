#include "redirect.h"

#include "mongoose.h"
#include "curl/curl.h"

void basic_to_bearer(struct mg_str basic_auth, char *bearer, size_t bearer_len) {
    basic_auth.buf += 6;
    basic_auth.len -= 6;

    char decoded[128] = {0};
    size_t decoded_len = sizeof(decoded);

    size_t len = mg_base64_decode(basic_auth.buf, basic_auth.len, decoded, decoded_len);
    if (len > 0 && len < decoded_len) {
        decoded[len] = '\0';
        mg_snprintf(bearer, bearer_len, "Bearer %s", decoded);
    } else {
        bearer[0] = '\0';
    }
}

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    char *response = (char *) userp;
    size_t current_len = strlen(response);
    size_t max_len = 8192;

    if (current_len + total_size >= max_len) {
        total_size = max_len - current_len - 1;
    }
    strncat(response, (char *) contents, total_size);
    return size * nmemb;
}

