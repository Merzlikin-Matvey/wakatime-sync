#include "redirect.h"
#include "string_utils.h"

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

void redirect(struct mg_http_message *hm, const char *redirect_url, char *response) {
    printf("Redirecting to %s\n", redirect_url);

    CURL *curl = curl_easy_init();

    if (!curl) {
        printf("Failed to initialize CURL\n");
        return;
    }

    char *method = mg_str_to_cstr(&hm->method);
    char *uri = mg_str_to_cstr(&hm->uri);
    char *path = strstr(uri, "://");
    if (path) {
        path = strchr(path + 3, '/');
        if (!path) {
            path = "/";
        }
    } else {
        path = uri;
    }
    char *query = mg_str_to_cstr(&hm->query);
    char full_url[1024];
    printf("URI: %s\n", uri);
    snprintf(full_url, sizeof(full_url), "%s%s%s%s", redirect_url, path, query[0] ? "?" : "", query);
    printf("Full URL: %s\n", full_url);

    struct curl_slist *headers = NULL;
    for (int i = 0; i < MG_MAX_HTTP_HEADERS && hm->headers[i].name.len > 0; i++) {
        char line[256];
        if (mg_strcmp(hm->headers[i].name, mg_str("Host")) == 0) {
            continue;
        } else if (mg_strcmp(hm->headers[i].name, mg_str("Authorization")) == 0) {
            char bearer[128];
            basic_to_bearer(hm->headers[i].value, bearer, sizeof(bearer));
            printf("Authorization header converted to Bearer: %s\n", bearer);
            snprintf(line, sizeof(line), "Authorization: %s", bearer);
        } else {
            snprintf(line, sizeof(line), "%.*s: %.*s",
                     (int) hm->headers[i].name.len, hm->headers[i].name.buf,
                     (int) hm->headers[i].value.len, hm->headers[i].value.buf);
        }
        headers = curl_slist_append(headers, line);
    }

    char *body = mg_str_to_cstr(&hm->body);

    printf("Method: %s\n", method);
    printf("Full URL: %s\n", full_url);
    printf("Headers:\n");
    for (struct curl_slist *h = headers; h != NULL; h = h->next) {
        printf("  %s\n", h->data);
    }
    printf("Body: %s\n", body);
    printf("Setting up CURL request...\n");

    curl_easy_setopt(curl, CURLOPT_URL, full_url);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    if (strcmp(method, "GET") != 0) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
    printf("Response: %s\n", response);



    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    free(method);
    free(uri);
    free(query);
    free(body);

}