#include "../include/helper.h"
#include <stdio.h>
// Helper functions
void flush_stdin()
{
  int ch;
  do
  {
    ch = getchar();
  } while (ch != '\n' && ch != EOF);
}


// updates string in place, ALWAYS pass copy of string
void remove_whitespace(char *str)
{
  if (str == NULL)
  {
    return; // Handle null pointer
  }
  char *src = str, *dst = str;
  while (*src)
  {
    if (!isspace((unsigned char)*src))
    {
      *dst++ = *src;
    }
    src++;
  }
  *dst = '\0';
}