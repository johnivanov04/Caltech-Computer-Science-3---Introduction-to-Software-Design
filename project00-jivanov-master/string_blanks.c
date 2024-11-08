#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **string_blanks(char *s) {
  // Implement this!
  // Allocate space for a list of strings.
  // (Hint: you need one string for each character that gets replaced by '_')

  // Use strcpy to copy the contents of s into a new string, setting one of
  // the characters in the copy to '_'.

  // Add the edited copy to the list of strings.

  // Return the final list of strings with blanks.
  int len = strlen(s);
  char **result = (char **)malloc(len * sizeof(char *));

  for (int i = 0; i < len; i++) {
    result[i] = (char *)malloc((len + 1) * sizeof(char));
    strcpy(result[i], s);
    result[i][i] = '_';
    result[i][len] = '\0';
  }
  return result;
}
