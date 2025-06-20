#pragma once

#include "mongoose.h"

void basic_to_bearer(struct mg_str basic_auth, char *bearer, size_t bearer_len);

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);

void redirect(struct mg_http_message *hm, const char *redirect_url, char *response);
