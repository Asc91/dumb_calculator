#include "../include/error.h"

char *get_error_message(err_t err) {
  switch (err) {
  case OK:
    return "OK";
  case DIVISION_BY_ZERO:
    return "Division by zero error.";
  case ERROR:
    return "An error occurred.";
  case INVALID_EXPRESSION:
    return "Invalid expression.";
  case OUT_OF_MEMORY:
    return "Out of memory.";
  case TOO_LONG_INPUT:
    return "Input too long.";
  case EMPTY_INPUT:
    return "EMPTY_INPUT";
  default:
    return "Unknown error.";
  }
}
