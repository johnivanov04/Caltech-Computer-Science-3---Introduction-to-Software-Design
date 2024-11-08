#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ll.h"


typedef struct node {
    entry_t entry;
    struct node *next;
} node_t;

typedef struct ll_map {
    size_t length;
    node_t *head;
} ll_map_t;

/**
 * Initialize a new node with the given key and value.
 * 
 * The returned node should be heap-allocated with malloc and it takes ownership
 * of the arguments so the caller should not modify or free them afterward.
 */
static node_t *node_init(char *key, char* value);

/**
 * Frees a linked list node and all nodes it points to.
 * 
 * Also frees the keys and values since they are owned by the node.
 * 
 * Passing NULL to this function is valid (and will do nothing).
 */
static void node_free(node_t *curr);

static node_t *node_init(char *key, char* value) {
    node_t *new_node = malloc(sizeof(node_t));
    
    
    // Allocate memory for the key and copy the value
    new_node->entry.key = key;
    

    // Allocate memory for the value and copy the value
    new_node->entry.value = value;

    // Set next pointer to NULL
    new_node->next = NULL;
    
    return new_node;
}


static void node_free(node_t *curr) {
    node_t *current = curr;
    node_t *previous = curr;
    while (current != NULL) {
        previous = current;
        current = current->next;
        free(previous->entry.key);
        free(previous->entry.value);
        free(previous);
    }

}

ll_map_t *ll_init(void) {
    ll_map_t *dict = malloc(sizeof(ll_map_t));
    dict->length = 0;
    dict->head = NULL;
    return dict;
}

void ll_free(ll_map_t *dict) {
    node_free(dict->head);
    free(dict);
}

char *ll_put(ll_map_t *dict, char *key, char *value) {
    if (!dict){
        return NULL;
    }

    if (!dict->head){
        node_t *new_node = node_init(key, value);
        dict->head = new_node;
        dict->length++;
        return NULL;
    }

    node_t *curr = dict->head;
    while(curr->next != NULL){
        if (strcmp(curr->entry.key, key) == 0){
            char *past = curr->entry.value;
            curr->entry.value = value;
            free(key);
            return past;
        }
        curr = curr->next;
    }

    if (strcmp(curr->entry.key, key) == 0){
        char *past = curr->entry.value;
        curr->entry.value = value;
        free(key);
        return past;
    }

    node_t *n_node = node_init(key, value);
    curr->next = n_node;
    dict->length++;
    return NULL;
}

char *ll_get(ll_map_t *dict, char *key) {
    if (!dict) {
        return NULL;
    }
    node_t *current = dict->head;
    while (current != NULL) {
        if (strcmp(current->entry.key, key) == 0) {
            return current->entry.value;
        }
        current = current->next;
    }
    return NULL;
}

strarray_t *ll_get_keys(ll_map_t *dict) {
    if (!dict || !dict->head || dict->length ==0){
        strarray_t *out = malloc(sizeof(strarray_t));
        if (!out){
            return NULL;
        }
        out->length = 0;
        out->data = NULL;
        return out;
    }
    strarray_t *keys = malloc(sizeof(strarray_t));
    keys -> length = dict->length;
    keys -> data = malloc(dict->length * sizeof(char *));
    node_t *curr = dict->head;
    int place = 0;
    while (curr != NULL){
        keys->data[place] = malloc((strlen(curr->entry.key)+1)*sizeof(char));
        strcpy(keys ->data[place], curr->entry.key);
        keys->data[place][strlen(curr->entry.key)] = '\0';
        curr = curr->next;
        place++;
    }
    return keys;
    
}
