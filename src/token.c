#include "../include/token.h"
#include "../include/error.h"
#include "../include/logger.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

const operator_t ops[] = {
    // should match sign enum
    {_add, add_sub, l_to_r},     {_sub, add_sub, l_to_r},
    {_mult, mult_div, l_to_r},   {_div, mult_div, l_to_r},
    {_r_shift, shift, l_to_r},   {_l_shift, shift, l_to_r},
    {_or, bit_or, l_to_r},       {_and, bit_and, l_to_r},
    {_neg, unary, r_to_l},       {_pos, unary, r_to_l},
    {_l_bracket, bracket, l_to_r},{_r_bracket, bracket, l_to_r},
    {_invalid_sign, 0, 0},
};

void stack_push(token_stack_t *top, token_t *token) {
  if (token == NULL) {
    LOG_WARN("\n>> Got null token %s:%d", __func__, __LINE__);
    return;
  }
  token->next = *top;
  *top = token;
}

token_t *stack_pop(token_stack_t *stack) {
  if (*stack== NULL) {
    LOG_WARN("\n>> Reached end of stack %s:%d", __func__, __LINE__);
    return NULL;
  }
  token_t *poped= *stack;
  *stack = (*stack)->next;
  return poped;
}

void init_token_queue(token_queue_t *q) {
  q->head = NULL;
  q->tail = NULL;
}

void enqueue(token_queue_t *q, token_t *token) {
  if (token == NULL) {
    LOG_WARN("\n>> Got null token %s:%d", __func__, __LINE__);
    return;
  }
  if (q->tail != NULL) {
    q->tail->next = token;
  } else {
    q->head = token; // if queue is empty, set head to new token
  }
  q->tail = token;
}

token_t *dequeue(token_queue_t *q) {
  if (q->head == NULL) {
    LOG_WARN("\n>> Queue is empty. %s:%d", __FILE__, __LINE__);
    return NULL;
  }
  token_t *deq = q->head;
  q->head = q->head->next;
  return deq;
}

void log_queue(token_queue_t *queue) {
  if (queue->head == NULL) {
    LOG_INFO("\n>> got null token queue");
    return;
  }
  token_t *tok = queue->head;
  LOG_INFO("\n>> Queue: ");
  while (tok != NULL) {
    if (tok->type == OPERAND) {
      LOG_INFO("N %lf ->", tok->val.num);
    } else if (tok->type == OPERATOR) {
      LOG_INFO("O %d ->", tok->val.op.s);
    } else if (tok->type == UNARY_OP) {
      LOG_INFO("O %d ->", tok->val.op.s);
    }
    tok = tok->next;
  }
}
void log_stack(token_stack_t *stack) {
  if (stack == NULL) {
    LOG_INFO("\n>> Empty stack");
    return;
  }
  token_t *tok = *stack;
  LOG_INFO("\n>> Stack: ");
  while (tok != NULL) {
    if (tok->type == OPERAND) {
      LOG_INFO("N %lf ->", tok->val.num);
    } else if (tok->type == OPERATOR) {
      LOG_INFO("O %d ->", tok->val.op.s);
    } else if (tok->type == UNARY_OP) {
      LOG_INFO("O %d ->", tok->val.op.s);
    }
    tok = tok->next;
  }
}

err_t is_unary(expression_t *ex, int *index) {
  if (*index == 0) // first char
  {
    return OK;
  } else if (!isdigit(*(*ex - 1)) && !isalpha(*(*ex - 1))) {
    return OK;
  }
  return ERROR;
}

err_t char_to_op(expression_t *ex, int *index, operator_t *op) {
  // for operators with multiple chars
  if (strncmp(*ex, ">>", 2) == 0) {
    LOG_INFO("Found right shift operator");
    (*index) += 2;
    *ex = *ex + 2;
    *op = ops[_r_shift];
    return OK;
  } else if (strncmp(*ex, "<<", 2) == 0) {
    (*index) += 2;
    *ex = *ex + 2;
    *op = ops[_l_shift];
    return OK;
  }

  // for operators with single char
  switch (**ex) {
  case '+':
    if(is_unary(ex, index) == OK) {
      *op = ops[_pos];
    } else {
      *op = ops[_add];
    }
    break;
  case '-':
    if (is_unary(ex, index) == OK) {
      *op = ops[_neg];
    } else {
      *op = ops[_sub];
    }
    break;
  case '*':
    *op = ops[_mult];
    break;
  case '/':
    *op = ops[_div];
    break;
  case '&':
    *op = ops[_and];
    break;
  case '|':
    *op = ops[_or];
    break;
  case '(':
    *op = ops[_l_bracket];
    break;
  case ')':
    *op = ops[_r_bracket];
    break;
  default:
    *op = ops[_invalid_sign];
    return INVALID_EXPRESSION;
  }
  (*index)++;
  (*ex)++;
  return OK;
}

err_t tokanizer(expression_t *ex, int *index, token_t *new_token) {
  if (ex == NULL) {
    LOG_WARN("\n>> Got null expression %s:%d", __func__, __LINE__);
    return INVALID_EXPRESSION;
  }

  if (isdigit(**ex)) {
    char *str_end;

    // Hex numbers and binary numbers
    if (strncmp(*ex, "0x", 2) == 0 || strncmp(*ex, "0X", 2) == 0) {
      long int hex = strtol(*ex, &str_end, 16);
      if (isalpha(*str_end)) // found non-hex character
      {
        return INVALID_EXPRESSION;
      }
      *index += (str_end - *ex); // update index
      *ex = str_end;
      new_token->type = OPERAND;
      new_token->val.hex_bin = hex;
      new_token->next = NULL;
      return OK;
    } else if (strncmp(*ex, "0b", 2) == 0 || strncmp(*ex, "0B", 2) == 0) {
      long int bin = strtol(*ex + 2, &str_end, 2);
      if (isdigit(*str_end)) {
        return INVALID_EXPRESSION;
      }
      *index += (str_end - *ex); // update index
      *ex = str_end;
      new_token->type = OPERAND;
      new_token->val.hex_bin = bin;
      new_token->next = NULL;
      return OK;
    } else {
      // floating / decimal numbers
      double fp = strtod(*ex, &str_end);
      *index += (str_end - *ex); // update index
      *ex = str_end;
      new_token->type = OPERAND;
      new_token->val.num = fp;
      new_token->next = NULL;
      return OK;
      // TO DO long integer if double overflows
    }
  } else if (isalpha(**ex)) {
    (*ex)++;
    (*index)++;
    return INVALID_EXPRESSION;
  } else if (**ex == ' ') {
    (*ex)++; // ignore white space
    (*index)++;
    return ERROR;
  } else {
    // special chars / operators
    operator_t op;

    if (char_to_op(ex, index, &op) == OK)// this will handle increamenting ex pointer.
    {
      if (op.s == _neg || op.s == _pos) {
        new_token->type = UNARY_OP;
      } else if (op.s == _l_bracket || op.s == _r_bracket) {
        new_token->type = BRACKET;
      } else {
        new_token->type = OPERATOR;
      }
    }
    new_token->val.op = op;
    new_token->next = NULL;
    return OK;
  }
  return INVALID_EXPRESSION;
}
