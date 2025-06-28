#include "../include/token.h"
#include "../include/helper.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

const operator_t ops[] = {
    // should match sign enum
    {_add, add_sub, l_to_r},     {_sub, add_sub, l_to_r},
    {_mult, mult_div, l_to_r},   {_div, mult_div, l_to_r},
    {_r_shift, shift, l_to_r},   {_l_shift, shift, l_to_r},
    {_or, bit_or, l_to_r},       {_and, bit_and, l_to_r},
    {_neg, unary_minus, r_to_l}, {_pos, unary_plus, r_to_l},
    {_invalid_sign, 0, 0},
};

void stack_push(token_stack_t *top, token_t *token) {
  token->next = *top;
  *top = token;
}

token_t *stack_pop(token_stack_t *top) {
  if (*top == NULL) {
    printf("\n>> reached end of stack");
    return NULL;
  }
  token_t *pop = *top;
  *top = (*top)->next;
  return pop;
}

void init_token_queue(token_queue_t *q) {
  q->head = NULL;
  q->tail = NULL;
}

void enqueue(token_queue_t *q, token_t *token) {
  if (q->tail != NULL) {
    q->tail->next = token;
  } else {
    q->head = token; // if queue is empty, set head to new token
  }
  q->tail = token;
}

token_t *dequeue(token_queue_t *q) {
  if (q->head == NULL) {
    LOG_INFO("\n>> Queue is empty. %s", __func__);
    return NULL;
  }
  token_t *deq = q->head;
  q->head = q->head->next;
  return deq;
}

bool is_unary(expression_t *ex, int *index) {
  if (*index == 0) // first char
  {
    return true;
  } else if (!isdigit(*(*ex - 1)) && !isalpha(*(*ex - 1))) {
    return true;
  }
  return false;
}

operator_t char_to_op(expression_t * ex, int *index) {
  // for operators with multiple chars
  if (strncmp(*ex, ">>", 2) == 0) {
    (*index) += 2;
    *ex = *ex + 2;
    return ops[_r_shift];
  } else if (strncmp(*ex, "<<", 2) == 0) {
    (*index) += 2;
    *ex = *ex + 2;
    return ops[_l_shift];
  }

  // for operators with single char
  operator_t result = ops[_invalid_sign];
  switch (**ex) {
  case '+':
    result = is_unary(ex, index) ? ops[_pos] : ops[_add];
    break;
  case '-':
    result = is_unary(ex, index) ? ops[_neg] : ops[_sub];
    break;
  case '*':
    result = ops[_mult];
    break;
  case '/':
    result = ops[_div];
    break;
  case '&':
    result = ops[_and];
    break;
  case '|':
    result = ops[_or];
    break;
  default:
    result = ops[_invalid_sign];
    break;
  }
  (*index)++;
  (*ex)++;
  return result;
}

token_t *tokanizer(expression_t *ex, int *index) {
  if (ex == NULL) {
    printf("\n>> got null expression");
    return NULL;
  }
  token_t *new_token = malloc(sizeof(token_t));
  if (isdigit(**ex)) {
    char *str_end;

    // Hex numbers and binary numbers
    if (strncmp(*ex, "0x", 2) == 0 || strncmp(*ex, "0X", 2) == 0) {
      long int hex = strtol(*ex, &str_end, 16);
      if (isalpha(*str_end)) // found non-hex character
      {
        return NULL;
      }
      *index += (str_end - *ex); // update index
      *ex = str_end;
      new_token->type = OPERAND;
      new_token->val.hex_bin = hex;
      new_token->next = NULL;
      return new_token;
    } else if (strncmp(*ex, "0b", 2) == 0 || strncmp(*ex, "0B", 2) == 0) {
      long int bin = strtol(*ex + 2, &str_end, 2);
      if (isdigit(*str_end)) {
        return NULL;
      }
      *index += (str_end - *ex); // update index
      *ex = str_end;
      new_token->type = OPERAND;
      new_token->val.hex_bin = bin;
      new_token->next = NULL;
      return new_token;
    } else {
      // floating / decimal numbers
      double fp = strtod(*ex, &str_end);
      *index += (str_end - *ex); // update index
      *ex = str_end;
      new_token->type = OPERAND;
      new_token->val.num = fp;
      new_token->next = NULL;
      return new_token;
      // TO DO long integer if double overflows
    }
  } else if (isalpha(**ex)) {
    (*ex)++;
    (*index)++;
    return NULL;
  } else if (**ex == ' ') {
    (*ex)++; // ignore white space
    (*index)++;
    return NULL;
  } else {
    // special chars / operators
    operator_t op =
        char_to_op(ex, index); // this will handle increamenting ex pointer.
    if (op.s == _invalid_sign) {
      printf("\n>> Invalid operator: %c", **ex);
      free(new_token);
      return NULL;
    }
    if (op.s == _neg || op.s == _pos) {
      new_token->type = UNARY_OP;
    } else {
      new_token->type = OPERATOR;
    }
    new_token->val.op = op;
    new_token->next = NULL;
    return new_token;
  }
  free(new_token);
  return NULL;
}
