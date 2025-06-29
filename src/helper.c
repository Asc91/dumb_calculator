#include "../include/helper.h"
#include <stdio.h>
// Helper functions
void flush_stdin() {
  int ch;
  do {
    ch = getchar();
  } while (ch != '\n' && ch != EOF);
}
