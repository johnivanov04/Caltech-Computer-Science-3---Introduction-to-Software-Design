#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

#include "network_util.h"
#include "http_request.h"
#include "http_response.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "USAGE:  %s <server port>\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);
    
    
    connection_t *connection;
    while (1){
        connection = nu_wait_client(port);
        
        
        char *request_str = nu_try_read_header(connection);
        while (!request_str){
            request_str = nu_try_read_header(connection);
        }
        request_t *request = request_parse(request_str);
        if(strcmp((request->path), "/hello") == 0){
            char *response_content = "Hello, world!";
            char *response = response_format(HTTP_OK, response_content);
            nu_send_str(connection, response);
        }
        else{
            char *response_content = "Hello, world!";
            char *response = response_format(HTTP_OK, response_content);
            nu_send_str(connection, response);
        }
        
        free(request_str);
        nu_close_connection(connection);
    }
    return 0;
}
