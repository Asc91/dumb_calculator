#include "../include/eval.h"
#include "../include/token.h"
#include "../include/logger.h"
#include "../include/rpn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

err_t eval_rpn(token_queue_t *queue, token_t *result) {
  if (queue->head == NULL) {
    LOG_WARN("\n>> Got empty token queue %s:%d", __FILE__, __LINE__);
    return INVALID_EXPRESSION;
  }
  token_stack_t stack = NULL;
  token_t *tok;
  while ((tok = dequeue(queue)) != NULL) {
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
      if ((right = stack_pop(&stack)) == NULL) {
        printf("\n>> Invalid expression");
        free(intermidiate);
        return INVALID_EXPRESSION;
      }
      if (tok->type == OPERATOR) {
        if ((left = stack_pop(&stack)) == NULL) {
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
  token_t *tmp = stack_pop(&stack);
  if (tmp == NULL) {
    LOG_ERROR("\n>> Err: Something went wrong, %s:%d", __FILE__, __LINE__);
    return ERROR;
  }
  *result = *tmp;
  return OK;
}

err_t eval_expression(char *expression, token_t *result) {
  if (expression == NULL ){
    return EMPTY_INPUT;
  }

  char * strend = strchr(expression, '\0');
  if (strend == NULL || strend == expression) {
    return EMPTY_INPUT;
  }

  size_t size = labs(expression - strend);
  if (size > INPUT_LIMIT) {
    return TOO_LONG_INPUT;
  }

  char *ex = malloc(size + 1);
  if (ex == NULL) {
    return OUT_OF_MEMORY;
  }

  memcpy(ex, expression, size + 1);
  char *ex_orig = ex;
  
  token_queue_t rpn; 
  err_t err = create_rpn(&ex, &rpn);
  free(ex_orig);
  if (err != OK) {
    return err;
  }

  err = eval_rpn(&rpn, result);
  if (err != OK) {
    return err;
  }
  
  return OK;
}
