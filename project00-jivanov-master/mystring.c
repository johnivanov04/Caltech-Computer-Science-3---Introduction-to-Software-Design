#include "mystring.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int count_words(const char *str) {
  int count = 0;
  int in_word = 0;

  while (*str != '\0') {
    if (*str == ' ') {
      in_word = 0;
    } else {
      if (!in_word) {
        count++;
        in_word = 1;
      }
    }
    str++;
  }
  //str[1]
  return count;
}

strarray_t *strsplit(const char *str) {
  // Count the total number of words
  int num_words = count_words(str);
  // Allocate memory for strarray object
  strarray_t *result = malloc(sizeof(strarray_t));

  // Initialize length and allocate memory for data array
  result->length = num_words;
  result->data = malloc(num_words * sizeof(char *));

  int word_index = 0;
  const char *start = str;
  const char *end = str;

  // Loop through the input string
  while (*end != '\0' && word_index < num_words) {
    // Skip leading whitespace
    while (*start == ' ') {
      start++;
    }

    // Find the end of the word
    end = start;
    while (*end != '\0' && *end != ' ') {
      end++;
    }

    // Allocate memory for the word and copy it
    int word_length = end - start;
    result->data[word_index] = malloc((word_length + 1) * sizeof(char));

    strncpy(result->data[word_index], start, word_length);
    result->data[word_index][word_length] = '\0';

    // Move to the next word
    word_index++;

    // Move to the next character
    start = end;
  }

  return result;
}