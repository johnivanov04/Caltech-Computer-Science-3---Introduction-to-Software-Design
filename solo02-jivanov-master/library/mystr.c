#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "strarray.h"
#include "mystr.h"

ssize_t mystr_indexof(const char *str, const char sep, size_t start) {
    if (str == NULL){
        return -1;
    }
    size_t length = strlen(str);
    if (start >= length){
        return -1;
    }

    for (size_t i = start; i < length; i++){
        if (str[i] == sep){
            return i;
        }
    }
    return -1;
}

strarray_t *mystr_split(const char *str, const char sep) {
    const char *walk = str;
    size_t d_count = 0;
    int d_bool = 0;
    while(*walk != '\0'){
        if (*walk == sep){
            d_bool = 0;
        }
        else{
            if (!d_bool){
                d_count++;
                d_bool = 1;
            }
        }
        walk ++;
    }
    
    strarray_t *returnVal = malloc(sizeof(strarray_t));
    returnVal->length = d_count;
    returnVal->data = malloc(d_count * sizeof(char*));
    size_t place = 0;
    const char *start = str;
    const char *end = str;
    while (place < d_count){
        while (*start == sep){
            start++;
        }

        end = start;
        while (*end != sep){
            if(*end == '\0'){
                break;
            }
            end ++;
        }
        size_t index = end -start;
        returnVal->data[place] = malloc((index+1)*sizeof(char));
        strncpy(returnVal->data[place], start, index);
        returnVal->data[place][index] = '\0';

        place++;
        start = end;
    }
    return returnVal;
}