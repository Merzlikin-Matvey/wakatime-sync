#pragma once

#include "mongoose.h"

void print_mg_str(struct mg_str s);

char *mg_str_to_cstr(const struct mg_str *s);