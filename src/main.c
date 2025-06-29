#include "../include/eval.h"
#include "../include/helper.h"
#include "../include/rpn.h"
#include "../include/token.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_SN64 9223372036854775808
#define MAX_SP64 9223372036854775807
#define MAX_U64 18446744073709551615

#define INPUT_LIMIT 25

enum commands { Convert, Eval, Exit, Help };

// main
int main() {
  printf("\n>> Welcome to Programming calculator!");
  printf("\n>> You are in eval mode. Use Help to get command list.");
  do {
    // Get input
    char input[INPUT_LIMIT] = {'\0'};
    printf("\n>> ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
      printf("\n>> Error reading input. Please try again.");
      flush_stdin();
      continue;
    }

    // Check for input buffer overflow
    char *str_end = strchr(input, '\n');
    if (str_end == NULL) {
      printf("\n>> Your expression was larger than %d characters.",
             INPUT_LIMIT);
      printf("\n>> Enter something smaller.");
      flush_stdin();
      continue;
    } else {
      *str_end = '\0';
    }

    // Check if input is command or expression.
    if (!strcmp(input, "Help")) {
      printf("\n>> 1. Eval: To evaluate expressions, supports arithmetic and "
             "bitwise operations.");
      printf("\n>> 2. Convert: To interconvert binary, hex and decimal");
      printf("\n>> 3. Exit: To exit programme.");
      printf("\n>> 4. Help: To print this text.");
    } else if (!strcmp(input, "Eval")) {
      printf("\n>> You are in eval mode. Enter expression to evaluate.");
    } else if (!strcmp(input, "Convert")) {
      printf("\n>> You are in convert mode. Enter value to convert.");
    } else if (!strcmp(input, "Exit")) {
      printf("\n>> Exiting Programme");
      return 0;
    } else {
      size_t size = labs(input - str_end);
      char *ex = malloc(size + 1);
      if (ex == NULL) {
        printf("\n>> Err: Out of memory, %s:%d", __FILE__, __LINE__);
        exit(2);
      }
      memcpy(ex, input, size + 1);
      char *ex_orig = ex;

      token_queue_t rpn; 
      err_t err = create_rpn(&ex, &rpn);
      if (err != OK) {
        if (err == INVALID_EXPRESSION){
          printf("\n>> Invalid expression. Please try again.");
        } else if (err == OUT_OF_MEMORY) {
          printf("\n>> Err: Out of memory, %s:%d", __FILE__, __LINE__);
          free(ex_orig);
          exit(2);
        } else {
          printf("\n>> Something went wrong, %s:%d", __FILE__, __LINE__);
        }
        free(ex_orig);
        continue;
      }
      free(ex_orig);

      log_rpn(&rpn);
      token_t *result;
      err = eval_rpn(&rpn, &result);
      if (err == OK) {
        printf("\n>> Result: %.2f\n", result->val.num);
        free(result);
      } else if (err == OUT_OF_MEMORY) {
        printf("\n>> Err: Out of memory");
        exit(2);
      } else if (err == DIVISION_BY_ZERO) {
        printf("\n>> Division by zero error");
      } else if (err == INVALID_EXPRESSION) {
        printf("\n>> Invalid expression. Please try again.");
      } else{
        printf("\n>> Something went wrong");
      }
      
     
      continue;
    
    }
    // fall
  } while (1);

  return 0;
}
