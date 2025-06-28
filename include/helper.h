#ifndef _HELPER_H_
#define _HELPER_H_

#include <stdio.h>
#include "../include/token.h"

#define DEBUG_RED "\x1b[31m"
#define DEBUG_GREEN "\x1b[32m"
#define DEBUG_YELLOW "\x1b[33m"
#define DEBUG_RESET "\x1b[0m"

#ifdef DEBUG
#define LOG_INFO(fmt, ...)                                                     \
  fprintf(stderr, DEBUG_GREEN fmt "" DEBUG_RESET, ##__VA_ARGS__)

#define LOG_WARN(fmt, ...)                                                     \
  fprintf(stderr, DEBUG_YELLOW fmt "" DEBUG_RESET, ##__VA_ARGS__)

#define LOG_ERR(fmt, ...)                                                      \
  fprintf(stderr, DEBUG_RED fmt "" DEBUG_RESET, ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...)                                                     \
  do {                                                                         \
  } while (0);
#define LOG_WARN(fmt, ...)                                                     \
  do {                                                                         \
  } while (0);
#define LOG_ERR(fmt, ...)                                                      \
  do {                                                                         \
  } while (0);
#endif

void flush_stdin();
void log_rpn(token_queue_t *queue);

#endif
