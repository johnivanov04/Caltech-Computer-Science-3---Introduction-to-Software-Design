#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

#include "network_util.h"
#include "http_request.h"
#include "http_response.h"
#include "router.h"
#include "web_util.h"
#include <assert.h>

const size_t NUMBER = 3;
const size_t DICE_C = 6;
const char *HELLO_RESPONSE = "Hello World!";
const char *error_display = "INVALID";

bytes_t *hello_handler() {
    return response_type_format(HTTP_OK, MIME_HTML, bytes_init(strlen(HELLO_RESPONSE), 
    strdup(HELLO_RESPONSE)));
}

bytes_t *roll_handler() {
    int roll = (rand() % DICE_C) + 1;
    char body[8];
    snprintf(body, sizeof(body), "%d", roll);
    return response_type_format(HTTP_OK, MIME_HTML, bytes_init(strlen(body), strdup(body)));
}

bytes_t *default_handler(request_t *req) {
    char *r_path = wutil_get_resolved_path(req);
    if (!r_path) {
        return response_type_format(HTTP_FORBIDDEN, MIME_PLAIN, bytes_init(strlen(error_display), strdup(error_display)));
    }

    response_code_t check_path = wutil_check_resolved_path(r_path);
    if (check_path != HTTP_OK) {
        return response_type_format(check_path, MIME_PLAIN, bytes_init(strlen(error_display), strdup(error_display)));
    }

    FILE *file = fopen(r_path, "r");
    if (!file) {
        free(r_path);
        return response_type_format(HTTP_FORBIDDEN, MIME_PLAIN, bytes_init(strlen(error_display), strdup(error_display)));
    }

    ssize_t size = wutil_get_file_size(file);
    if (size < 0) {
        fclose(file);
        free(r_path);
        return response_type_format(HTTP_FORBIDDEN, MIME_PLAIN, bytes_init(strlen(error_display), strdup(error_display)));
    }

    char *contents = malloc(size + 1);
    assert(contents!=NULL);
    fread(contents, 1, size, file);
    contents[size] = '\0';
    fclose(file);

    char *ext = wutil_get_filename_ext(r_path);
    mime_type_t mime = wutil_get_mime_from_extension(ext);
    free(r_path);

    return response_type_format(HTTP_OK, mime, bytes_init(size, contents));
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "USAGE:  %s <server port>\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);

    router_t *router = router_init(NUMBER, (route_handler_t)(default_handler));
    router_register(router, "/hello", hello_handler);
    router_register(router, "/roll", roll_handler);

    while (1) {
        connection_t *connection = nu_wait_client(port);

        char *request_str = NULL;
        while (request_str == NULL) {
            request_str = nu_try_read_header(connection);
        }
        printf("%s", request_str);

        request_t *req = request_parse(request_str);
        free(request_str);

        bytes_t *response = router_dispatch(router, req);
        nu_send_bytes(connection, response->data, response->len);
        bytes_free(response);

        nu_close_connection(connection);
    }
    router_free(router);
    return 0;
}