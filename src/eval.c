#include "../include/eval.h"
#include "../include/token.h"
#include <stdlib.h>
#include <stdio.h>

token_val_t eval_op(double left, double right, operator_t op) {
  token_val_t result;
  switch (op.s) {
  case _add:
    result.num = left + right;
    break;
  case _sub:
    result.num = left - right;
    break;
  case _mult:
    result.num = left * right;
    break;
  case _div:
    if (right == 0) {
      printf("\n>> Division by zero error");
      result.num = 0; // or handle error
    } else {
      result.num = left / right;
    }
    break;
  case _r_shift:
    result.num = (long)left >> (long)right;
    break;
  case _l_shift:
    result.num = (long)left << (long)right;
    break;
  case _or:
    result.num = (long)left | (long)right;
    break;
  case _and:
    result.num = (long)left & (long)right;
    break;
  case _neg:
    result.num = -right;
    break;
  case _pos:
    result.num = right;
    break;
  default:
    printf("\n>> Invalid operator");
    result.num = 0; // or handle error
  }
  return result;
}

token_t *eval_rpn(token_queue_t *queue) {
  if (queue->head == NULL) {
    printf("\n>> got null token queue");
    return NULL;
  }
  token_stack_t stack = NULL;
  token_t *tok;
  while ((tok = dequeue(queue)) != NULL) {
    if (tok->type == OPERAND) {
      stack_push(&stack, tok);
    } else if (tok->type == OPERATOR) {
      token_t *right = stack_pop(&stack);
      token_t *left = stack_pop(&stack);
      if (left == NULL || right == NULL) {
        printf("\n>> invalid expression");
        return NULL;
      }
      token_t *result = malloc(sizeof(token_t));
      result->type = OPERAND;
      result->val = eval_op(left->val.num, right->val.num, tok->val.op);
      stack_push(&stack, result);
      free(left);
      free(right);
    } else if (tok->type == UNARY_OP) {
      token_t *right = stack_pop(&stack);
      if (right == NULL) {
        printf("\n>> invalid expression");
        return NULL;
      }
      token_t *result = malloc(sizeof(token_t));
      result->type = OPERAND;
      result->val = eval_op(0, right->val.num, tok->val.op);
      stack_push(&stack, result);
      free(right);
    }
  }
  if (stack == NULL || stack->next != NULL || stack->type != OPERAND) {
    printf("\n>> invalid expression");
    return NULL;
  }
  return stack_pop(&stack);
}
