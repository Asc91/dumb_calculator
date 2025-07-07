#ifndef _TOKEN_H_
#define _TOKEN_H_

#include "../include/error.h"
#include <stdbool.h>

typedef enum sign {
  _add,
  _sub,
  _mult,
  _div,
  _r_shift,
  _l_shift,
  _or,
  _and,
  _neg,
  _pos,
  _l_bracket,
  _r_bracket,
  _invalid_sign,
} sign_t;

typedef enum precedence // should be ascending order of precedence
{ bit_or,
  bit_xor,
  bit_and,
  shift,
  add_sub,
  mult_div,
  unary,
  bracket,
} precedence_t;

typedef enum associativity_t {
  l_to_r,
  r_to_l,
} associativity;

typedef struct op {
  sign_t s;
  precedence_t p;
  associativity a;
} operator_t;

typedef enum token_type {
  BRACKET,
  UNARY_OP,
  OPERATOR,
  OPERAND,
} token_type_t;

typedef union token_val {
  double num;
  long int hex_bin;
  operator_t op;
} token_val_t;

typedef struct token {
  token_type_t type;
  token_val_t val;
  struct token *next;
} token_t;

typedef token_t *token_stack_t;

typedef struct token_queue {
  token_t *head;
  token_t *tail;
} token_queue_t;

typedef char *expression_t;

void stack_push(token_stack_t *top, token_t *token);

token_t *stack_pop(token_stack_t *top);

void init_token_queue(token_queue_t *q);

void enqueue(token_queue_t *q, token_t *token);

token_t *dequeue(token_queue_t *q);

void log_queue(token_queue_t *queue);

void log_stack(token_stack_t *stack);

err_t is_unary(expression_t *ex, int *index);

err_t char_to_op(expression_t *ex, int *index, operator_t *op);

err_t tokanizer(expression_t *ex, int *index, token_t *new_token);

#endif