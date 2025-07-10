#include "../include/eval.h"
#include "../include/token.h"
#include "../include/logger.h"
#include "../include/rpn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

err_t eval_binary_op(operand_t left, operand_t right, operator_t op, token_val_t *result)
{
  if (left.type == DECIMAL || right.type == DECIMAL || op.s == MULT || op.s == DIV)
  {
    result->operand.type = DECIMAL;
    double left_dec = left.type == DECIMAL ? left.val.dec : (double)left.val.hex_bin;
    double right_dec = right.type == DECIMAL ? right.val.dec : (double)right.val.hex_bin;

    switch(op.s)
    {
    case ADD:
      result->operand.val.dec = left_dec + right_dec;
      break;
    case SUB:
      result->operand.val.dec = left_dec - right_dec;
      break;
    case MULT:
      result->operand.val.dec = left_dec * right_dec;
      break;
    case DIV:
      if (abs(right_dec) == 0)
      {
        LOG_WARN("\n>> Division by zero error, %s:%d", __FILE__, __LINE__);
        return DIVISION_BY_ZERO;
      }
      else
      {
        result->operand.val.dec = left_dec / right_dec;
      }
      break;
    case R_SHIFT:
      result->operand.val.dec = (double)((uint64_t)left_dec >> (uint64_t)right_dec);
      break;
    case L_SHIFT:
      result->operand.val.dec = (double)((uint64_t)left_dec << (uint64_t)right_dec);
      break;
    case OR:
      result->operand.val.dec = (double)((uint64_t)left_dec | (uint64_t)right_dec);
      break;
    case AND:
      result->operand.val.dec = (double)((uint64_t)left_dec & (uint64_t)right_dec);
      break;
    default:
      LOG_WARN("\n>> Invalid operator %s:%d", __FILE__, __LINE__);
      return INVALID_EXPRESSION;
    }
    return OK;
  } else {
    result->operand.type = HEXADECIMAL;
    uint64_t left_hex = left.type == HEXADECIMAL ? left.val.hex_bin : (uint64_t)left.val.dec;
    uint64_t right_hex = right.type == HEXADECIMAL ? right.val.hex_bin : (uint64_t)right.val.dec;

    switch(op.s)
    {
    case ADD:
      result->operand.val.hex_bin = left_hex + right_hex;
      break;
    case SUB:
      result->operand.val.hex_bin = left_hex - right_hex;
      break;
    case R_SHIFT:
      result->operand.val.hex_bin = left_hex >> right_hex;
      break;
    case L_SHIFT:
      result->operand.val.hex_bin = left_hex << right_hex;
      break;
    case OR:
      result->operand.val.hex_bin = left_hex | right_hex;
      break;
    case AND:
      result->operand.val.hex_bin = left_hex & right_hex;
      break;
    default:
      LOG_WARN("\n>> Invalid operator %s:%d", __FILE__, __LINE__);
      return INVALID_EXPRESSION;
    }
    return OK;
  }
  return OK;
}

err_t eval_unary_op(operand_t right, operator_t op, token_val_t *result)
{
  if (right.type == DECIMAL)
  {
    result->operand.type = DECIMAL;
    switch (op.s)
    {
    case NEG:
      result->operand.val.dec = -right.val.dec;
      break;
    case POS:
      result->operand.val.dec = right.val.dec;
      break;
    default:
      LOG_WARN("\n>> Invalid operator %s:%d", __FILE__, __LINE__);
      return INVALID_EXPRESSION;
    }
  }
  else
  {
    result->operand.type = HEXADECIMAL;
    switch (op.s)
    {
    case NEG:
      result->operand.val.hex_bin = ~right.val.hex_bin + 1; // Two's complement for negation 
      break;
    case POS:
      result->operand.val.hex_bin = right.val.hex_bin;
      break;
    default:
      LOG_WARN("\n>> Invalid operator %s:%d", __FILE__, __LINE__);
      return INVALID_EXPRESSION;
    }
  }
  return INVALID_EXPRESSION;
}


err_t eval_rpn(token_queue_t *queue, token_t *result)
{
  if (queue->head == NULL)
  {
    LOG_WARN("\n>> Got empty token queue %s:%d", __FILE__, __LINE__);
    return INVALID_EXPRESSION;
  }
  token_stack_t stack = NULL;
  token_t *tok;
  while ((tok = dequeue(queue)) != NULL)
  {
    if (tok->type == OPERAND)
    {
      stack_push(&stack, tok);
    }
    else if (tok->type == OPERATOR)
    {
      token_t *intermidiate = malloc(sizeof(token_t));
      if (intermidiate == NULL)
      {
        LOG_ERROR("\n>> Err: Out of memory, %s:%d", __FILE__, __LINE__);
        return OUT_OF_MEMORY;
      }

      /* Pop top operand by default (in case unary operator)
       *  then if it's a binary operator, pop another operand
       *  and evaluate the operation.
       */
      token_t *left = NULL, *right = NULL;
      err_t err = ERROR;
      if ((right = stack_pop(&stack)) == NULL)
      {
        printf("\n>> Invalid expression");
        free(intermidiate);
        return INVALID_EXPRESSION;
      }
      if (tok->val.op.type == BINARY_OP)
      {
        if ((left = stack_pop(&stack)) == NULL)
        {
          LOG_WARN("\n>> Invalid expression %s:%d", __FILE__, __LINE__);
          free(intermidiate);
          return INVALID_EXPRESSION;
        }
        err = eval_binary_op(left->val.operand, right->val.operand, tok->val.op, &intermidiate->val);
      } else {
        err = eval_unary_op(right->val.operand, tok->val.op, &intermidiate->val);
      }
      if (err != OK)
      {
        free(intermidiate);
        return err;
      }
      intermidiate->type = OPERAND;
      stack_push(&stack, intermidiate);
    }
  }
  /* Only one token should be left in the stack
   */
  if (stack == NULL || stack->next != NULL || stack->type != OPERAND)
  {
    if (stack == NULL)
    {
      LOG_WARN("\n>> Stack is empty");
    }
    else if (stack->next != NULL)
    {
      LOG_WARN("\n>> Stack has more than one element");
    }
    else if (stack->type != OPERAND)
    {
      LOG_WARN("\n>> Top of stack is not an operand");
    }
    LOG_WARN("\n>> Invalid expression");
    return INVALID_EXPRESSION;
  }
  token_t *tmp = stack_pop(&stack);
  if (tmp == NULL)
  {
    LOG_ERROR("\n>> Err: Something went wrong, %s:%d", __FILE__, __LINE__);
    return ERROR;
  }
  *result = *tmp;
  return OK;
}

err_t eval_expression(char *expression, token_t *result)
{
  if (expression == NULL)
  {
    return EMPTY_INPUT;
  }

  char *strend = strchr(expression, '\0');
  if (strend == NULL || strend == expression)
  {
    return EMPTY_INPUT;
  }

  size_t size = labs(expression - strend);
  if (size > INPUT_LIMIT)
  {
    return TOO_LONG_INPUT;
  }

  char *ex = malloc(size + 1);
  if (ex == NULL)
  {
    return OUT_OF_MEMORY;
  }

  memcpy(ex, expression, size + 1);
  char *ex_orig = ex;

  token_queue_t rpn;
  err_t err = create_rpn(&ex, &rpn);
  free(ex_orig);
  if (err != OK)
  {
    return err;
  }
  log_queue(&rpn);

  err = eval_rpn(&rpn, result);
  if (err != OK)
  {
    return err;
  }

  return OK;
}
