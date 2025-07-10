#include "../include/token.h"
#include "../include/error.h"
#include "../include/logger.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

const operator_t ops[] = {
    // should match sign enum
    {ADD, ADD_SUB, L_TO_R, BINARY_OP},     {SUB, ADD_SUB, L_TO_R, BINARY_OP},
    {MULT, MULT_DIV, L_TO_R, BINARY_OP},   {DIV, MULT_DIV, L_TO_R, BINARY_OP},
    {R_SHIFT, SHIFT, L_TO_R, BINARY_OP},   {L_SHIFT, SHIFT, L_TO_R, BINARY_OP},
    {OR, BIT_OR, L_TO_R, BINARY_OP},       {AND, BIT_AND, L_TO_R, BINARY_OP},
    {NEG, UNARY, R_TO_L, UNARY_OP},       {POS, UNARY, R_TO_L, UNARY_OP},
    {INVALID_SIGN, INVALID_P, INVALID_A, INVALID_OP},
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
      if (tok->val.operand.type == HEXADECIMAL) {
        LOG_INFO("N 0x%lx ->", tok->val.operand.val.hex_bin);
      } else if (tok->val.operand.type == BINARY) {
        // To do: implement binary logging
        LOG_INFO("N 0x%lx ->", tok->val.operand.val.hex_bin);
      } else if (tok->val.operand.type == DECIMAL)
      {
        LOG_INFO("N %.2lf ->", tok->val.operand.val.dec);
      }
    } else if (tok->type == OPERATOR) {
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
      if (tok->val.operand.type == HEXADECIMAL) {
        LOG_INFO("N 0x%lx ->", tok->val.operand.val.hex_bin);
      } else if (tok->val.operand.type == BINARY) {
        // To do: implement binary logging
        LOG_INFO("N 0x%lx ->", tok->val.operand.val.hex_bin);
      } else if (tok->val.operand.type == DECIMAL) {
        LOG_INFO("N %.2lf ->", tok->val.operand.val.dec);
      }
    } else if (tok->type == OPERATOR) {
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

err_t char_to_token(expression_t *ex, int *index, token_t *new_token) {
  // everything except brackets is operator here
  new_token->type = OPERATOR;

  // for operators with multiple chars
  if (strncmp(*ex, ">>", 2) == 0) {
    LOG_INFO("Found right shift operator");
    (*index) += 2;
    *ex = *ex + 2;
    new_token->val.op = ops[R_SHIFT];
    return OK;
  } else if (strncmp(*ex, "<<", 2) == 0) {
    (*index) += 2;
    *ex = *ex + 2;
    new_token->val.op = ops[L_SHIFT];
    return OK;
  }

  // for operators with single char
  switch (**ex) {
  case '+':
    if(is_unary(ex, index) == OK) {
      new_token->val.op = ops[POS];
    } else {
      new_token->val.op = ops[ADD];
    }
    break;
  case '-':
    if (is_unary(ex, index) == OK) {
      new_token->val.op = ops[NEG];
    } else {
      new_token->val.op = ops[SUB];
    }
    break;
  case '*':
    new_token->val.op = ops[MULT];
    break;
  case '/':
    new_token->val.op = ops[DIV];
    break;
  case '&':
    new_token->val.op = ops[AND];
    break;
  case '|':
    new_token->val.op = ops[OR];
    break;
  case '(':
    new_token->type = BRACKET;
    new_token->val.bracket = L_BRACKET;
    break;
  case ')':
    new_token->type = BRACKET;
    new_token->val.bracket = R_BRACKET;
    break;
  default:
   new_token->val.op = ops[INVALID_SIGN];
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
      uint64_t hex = strtoull(*ex, &str_end, 16);
      if (isalpha(*str_end)) // found non-hex character
      {
        return INVALID_EXPRESSION;
      }
      *index += (str_end - *ex); // update index
      *ex = str_end;
      new_token->type = OPERAND;
      new_token->val.operand.type = HEXADECIMAL;
      new_token->val.operand.val.hex_bin = hex;
      new_token->next = NULL;
      return OK;
    } else if (strncmp(*ex, "0b", 2) == 0 || strncmp(*ex, "0B", 2) == 0) {
      uint64_t bin = strtoull(*ex + 2, &str_end, 2);
      if (isdigit(*str_end)) {
        return INVALID_EXPRESSION;
      }
      *index += (str_end - *ex); // update index
      *ex = str_end;
      new_token->type = OPERAND;
      new_token->val.operand.type = BINARY;
      new_token->val.operand.val.hex_bin = bin;
      new_token->next = NULL;
      return OK;
    } else {
      // floating / decimal numbers
      double fp = strtod(*ex, &str_end);
      *index += (str_end - *ex); // update index
      *ex = str_end;
      new_token->type = OPERAND;
      new_token->val.operand.type = DECIMAL;
      new_token->val.operand.val.dec = fp;
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
    if (char_to_token(ex, index, new_token) != OK)// this will handle increamenting ex pointer.
    {
      return INVALID_EXPRESSION;
    }
    new_token->next = NULL;
    return OK;
  }
  return INVALID_EXPRESSION;
}
