#ifndef _TOKEN_H_
#define _TOKEN_H_

#include "../include/error.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum sign {
  ADD,
  SUB,
  MULT,
  DIV,
  R_SHIFT,
  L_SHIFT,
  OR,
  AND,
  NEG,
  POS,
  INVALID_SIGN,
} sign_t;

typedef enum precedence // should be ascending order of precedence
{ BIT_OR,
  BIT_XOR,
  BIT_AND,
  SHIFT,
  ADD_SUB,
  MULT_DIV,
  UNARY,
  INVALID_P,
} precedence_t;

typedef enum associativity_t {
  L_TO_R,
  R_TO_L,
  INVALID_A,
} associativity;

typedef enum operator_type {
  UNARY_OP,
  BINARY_OP,
  INVALID_OP,
} operator_type_t;

typedef struct op {
  sign_t s;
  precedence_t p;
  associativity a;
  operator_type_t type;
} operator_t;

typedef enum operand_type {
  DECIMAL,
  HEXADECIMAL,
  BINARY,
} operand_type_t;

typedef union operand_val {
  double dec;        
  uint64_t hex_bin;  
} operand_val_t;

typedef struct operand {
  operand_val_t val;
  operand_type_t type;
} operand_t;

typedef enum token_type {
  BRACKET,
  OPERATOR,
  OPERAND,
} token_type_t;

typedef enum bracket {
  L_BRACKET,
  R_BRACKET,
} bracket_t;

typedef union token_val {
  bracket_t bracket;
  operator_t op;
  operand_t operand;
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

err_t char_to_token(expression_t *ex, int *index, token_t *new_token);

err_t tokanizer(expression_t *ex, int *index, token_t *new_token);

#endif