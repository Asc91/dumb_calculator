#ifndef _ERROR_H_
#define _ERROR_H_

#define INPUT_LIMIT 25

typedef enum errors {
  OK,
  DIVISION_BY_ZERO,
  ERROR,
  INVALID_EXPRESSION,
  OUT_OF_MEMORY,
  TOO_LONG_INPUT,
  EMPTY_INPUT
} err_t;

char *get_error_message(err_t err);
#endif
