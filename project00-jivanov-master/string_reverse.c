#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *string_reverse(char *s) {
  // Implement this!
  // Use strlen to allocate enough memory for the string, including the
  // null-terminator.

  // Set the null-terminator byte.

  // Write a for loop to reverse the string. You can index into the
  // string just like an array to get the characters of s as well as
  // set the characters in the reversed string.

  // Return the reversed string.
  size_t len = strlen(s);
  char *reversed = malloc(sizeof(char) * (len + 1));

  for (size_t i = 0; i < len; i++) {
    reversed[i] = s[len - i - 1];
  }
  reversed[len] = '\0';

  return reversed;
}
