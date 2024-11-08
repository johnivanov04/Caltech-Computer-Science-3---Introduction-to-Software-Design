#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "http_request.h"
#include "ll.h"
#include "mystr.h"

request_t *request_init(const char *method, const char *path , const char *http_version) {
    request_t *req = malloc(sizeof(request_t));
    if (req == NULL){
        perror("Failed to allocate memory for request");
        return NULL;
    }

    req->method = strdup(method);
    if (req->method == NULL){
        free(req);
        perror("Failed to copy method");
        return NULL;
    }
    req->path = strdup(path);
    if (req->path == NULL){
        free(req->method);
        free(req);
        perror("Failed to copy path");
        return NULL;
    }

    req->http_version = strdup(http_version);
    if (req->http_version == NULL){
        free(req->path);
        free(req->method);
        free(req);
        perror("Failed to copy HTTP version");
        return NULL;
    }

    req->headers = ll_init();
    if (req->headers == NULL){
        free(req->http_version);
        free(req->path);
        free(req->method);
        free(req);
        perror("Failed to create headers list");
        return NULL;
    }
    return req;
}




request_t *request_parse(const char *contents) {
    const char *iterate_cont = contents;
    ssize_t start_end = mystr_indexof(iterate_cont, '\r', 0);
    char *start_line = malloc((start_end + 1) * sizeof(char));
    strncpy(start_line, iterate_cont, start_end);
    start_line[start_end] = '\0';
    strarray_t *split_by_space = mystr_split(start_line,' ');

    request_t *return_val = request_init(split_by_space->data[0], split_by_space->data[1], split_by_space->data[2]);
    strarray_free(split_by_space);
    free(start_line);

    iterate_cont = iterate_cont + start_end +2;
    strarray_t *number_iterations = mystr_split(iterate_cont, '\n');
    for (size_t i = 0; i < number_iterations->length-1; i++){
        ssize_t head_end = mystr_indexof(iterate_cont, '\n', 0);
        if (head_end == -1){
            break;
        }
        char *head_line = malloc((head_end + 1) * sizeof(char));
        strncpy(head_line, iterate_cont, head_end);
        head_line[head_end] = '\0';
        ssize_t colon_index = mystr_indexof(head_line, ':', 0);
        if (colon_index == -1 || colon_index+2 >= head_end){
            free(head_line);
            continue;
        }
        char *key = calloc((colon_index+1), sizeof(char));
        strncpy(key, head_line, colon_index);
        ssize_t start_value = colon_index + 2;
        ssize_t end_value = head_end - 2;

        char *value = calloc((end_value - start_value + 2), sizeof(char));
        strncpy(value, head_line + start_value, end_value - start_value + 1);
        ll_put(return_val->headers, key, value);
        free(head_line);
        iterate_cont = iterate_cont + head_end + 1;
    }
    strarray_free(number_iterations);
    return return_val;
}

void request_free(request_t *req) {
    if (req != NULL){
        free(req->method);
        free(req->path);
        free(req->http_version);

        ll_free(req->headers);

        free(req);
    }
}