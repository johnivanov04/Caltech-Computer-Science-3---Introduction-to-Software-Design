#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "strarray.h"

strarray_t *strarray_init(size_t length) {
    strarray_t *arr = malloc(sizeof(strarray_t));
    arr->length = length;
    arr->data = calloc(length, sizeof(char*));
    return arr;
}

void strarray_free(strarray_t *arr) {
    if (arr != NULL) {
        for (size_t i = 0; i < arr->length; i++) {
            free(arr->data[i]);
        }
        free(arr->data);
        free(arr);
    }
}
