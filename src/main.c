#include <stdio.h>
#include "mongoose.h"

static void event_handler(struct mg_connection *c, int event, void *ev_data) {
    if (event == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        if (mg_match(hm->uri, mg_str("/api/test"), NULL)) {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"message\": \"Hello, World!\"}");
        } else {
            mg_http_reply(c, 404, "Content-Type: text/plain\r\n", "Not Found");
        }
    }
}

int main(void) {
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    mg_http_listen(&mgr, "0.0.0.0:8080", event_handler, &mgr);
    for (;;) mg_mgr_poll(&mgr, 1000);
}