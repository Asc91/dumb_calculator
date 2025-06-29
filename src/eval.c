#include "../include/eval.h"
#include "../include/token.h"
#include "../include/logger.h"
#include <stdio.h>
#include <stdlib.h>

err_t eval_op(double left, double right, operator_t op, token_val_t *result) {
  switch (op.s) {
  case _add:
    result->num = left + right;
    break;
  case _sub:
    result->num = left - right;
    break;
  case _mult:
    result->num = left * right;
    break;
  case _div:
    if (abs(right) == 0) {
      LOG_WARN("\n>> Division by zero error, %s:%d", __FILE__, __LINE__);
      return DIVISION_BY_ZERO;
    } else {
      result->num = left / right;
    }
    break;
  case _r_shift:
    result->num = (long)left >> (long)right;
    break;
  case _l_shift:
    result->num = (long)left << (long)right;
    break;
  case _or:
    result->num = (long)left | (long)right;
    break;
  case _and:
    result->num = (long)left & (long)right;
    break;
  case _neg:
    result->num = -right;
    break;
  case _pos:
    result->num = right;
    break;
  default:
    LOG_WARN("\n>> Invalid operator %s:%d", __FILE__, __LINE__);
    return INVALID_EXPRESSION;
  }
  return OK;
}

err_t eval_rpn(token_queue_t *queue, token_t **result) {
  if (queue->head == NULL) {
    LOG_WARN("\n>> Got empty token queue %s:%d", __FILE__, __LINE__);
    return INVALID_EXPRESSION;
  }
  token_stack_t stack = NULL;
  token_t *tok;
  while (dequeue(queue, &tok) == OK) {
    if (tok->type == OPERAND) {
      stack_push(&stack, tok);
    } else {
      token_t *intermidiate = malloc(sizeof(token_t));
      if (intermidiate == NULL) {
        LOG_ERROR("\n>> Err: Out of memory, %s:%d", __FILE__, __LINE__);
        return OUT_OF_MEMORY;
      }

      /* Pop top operand by default (in case ternary operator)
      *  then if it's a arithmetic operator, pop another operand
      *  and evaluate the operation.
      */
      token_t *left = NULL, *right = NULL; 
      err_t err = ERROR;
      if (stack_pop(&stack, &right) != OK) {
        printf("\n>> Invalid expression");
        free(intermidiate);
        return INVALID_EXPRESSION;
      }
      if (tok->type == OPERATOR) {
        if (stack_pop(&stack, &left) != OK) {
          LOG_WARN("\n>> Invalid expression %s:%d", __FILE__, __LINE__);
          free(intermidiate);
          return INVALID_EXPRESSION;
        }
        err = eval_op(left->val.num, right->val.num, tok->val.op, &intermidiate->val);
      }else{
        err = eval_op(0, right->val.num, tok->val.op, &intermidiate->val);
      }
      if (err != OK) {
        free(intermidiate);
        return err;
      }
      intermidiate->type = OPERAND;
      stack_push(&stack, intermidiate);
    }
  }
  /* Only one token should be left in the stack
  */ 
   if (stack == NULL || stack->next != NULL || stack->type != OPERAND) {
    if (stack == NULL) {
      LOG_WARN("\n>> Stack is empty");
    } else if (stack->next != NULL) {
      LOG_WARN("\n>> Stack has more than one element");
    } else if (stack->type != OPERAND) {
      LOG_WARN("\n>> Top of stack is not an operand");
    }
    LOG_WARN("\n>> Invalid expression");
    return INVALID_EXPRESSION;
  }
  if (stack_pop(&stack, result) != OK) {
    LOG_ERROR("\n>> Err: Something went wrong, %s:%d", __FILE__, __LINE__);
    return ERROR;
  }
  return OK;
}
