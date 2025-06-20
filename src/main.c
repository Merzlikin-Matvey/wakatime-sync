#include "redirect.h"
#include "string_utils.h"

#include <stdio.h>
#include "mongoose.h"
#include "curl/curl.h"


const char *REDIRECT_URL = "https://hackatime.hackclub.com/api/hackatime/v1";



void request_info(struct mg_http_message *hm) {
    printf("===========[ Request ]===========\n");
    print_mg_str(hm->method);
    print_mg_str(hm->uri);
    print_mg_str(hm->query);
    print_mg_str(hm->body);
    printf("===================================\n");
}


static void event_handler(struct mg_connection *c, int event, void *ev_data) {
    if (event == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        if (mg_strcmp(hm->uri, mg_str("/users/current/heartbeat")) == 0 && mg_strcmp(hm->method, mg_str("POST")) == 0) {
            printf(hm->body.buf);
            mg_http_reply(c, 200, "Content-Type: text/plain\r\n", "OK\n");
        } else {
            printf("REDIRECTING TO HACKTIME API\n");
            request_info(hm);
            char response[4096] = {0};
            redirect(hm, REDIRECT_URL, response);
            printf("Response from redirect: %s\n", response);
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", response);
        }
    }
}


int main(void) {
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    mg_http_listen(&mgr, "0.0.0.0:8080", event_handler, &mgr);
    for (;;) mg_mgr_poll(&mgr, 1000);
}


