#include "../include/rpn.h"
#include "../include/logger.h"
#include "../include/token.h"
#include "../include/error.h"
#include <stdlib.h>

err_t create_rpn(expression_t *ex, token_queue_t *queue) {
  if (ex == NULL) {
    LOG_WARN("\n>> Got null expression %s:%d", __FILE__, __LINE__);
    return INVALID_EXPRESSION;
  }
  token_stack_t stack = NULL;
  init_token_queue(queue);
  int index = 0;
  do {
    token_t *tok = malloc(sizeof(token_t));
    if (tok == NULL){
      LOG_ERROR("\n>> Err: Out of memory, %s:%d", __FILE__, __LINE__);
      return OUT_OF_MEMORY;
    }
    err_t err;
    if ((err = tokanizer(ex, &index, tok)) != OK){
      free(tok);
      if (err == INVALID_EXPRESSION){
        LOG_ERROR("\n>> Err: Invalid expression, %s:%d", __FILE__, __LINE__);
        return err;
      }
      continue;
    }

    if (tok->type == OPERAND) {
      enqueue(queue, tok);
    } else if (tok->type == BRACKET) {
      if (tok->val.op.s == _l_bracket) {
        stack_push(&stack, tok);
      } else {
        log_stack(&stack);
        log_queue(queue);
        token_t *tmp = NULL;
        while (1) {
          if (stack_pop(&stack, &tmp) != OK) {
            LOG_ERROR(
                "\n>> Should have found l_bracket before stack end, %s:%d",
                __FILE__, __LINE__);
            free(tok);
            return INVALID_EXPRESSION;
          }
          LOG_INFO("\n>> op_s %d", tmp->val.op.s);
          if (tmp->val.op.s != _l_bracket) {
            enqueue(queue, tmp);
          } else {
            break; // got _l_breaket removed from stack
          }
        }
      }
    } else if (tok->type == OPERATOR || tok->type == UNARY_OP) {
      token_t * tmp;
      while (stack != NULL && stack->type != BRACKET) {
        if ((tok->val.op.p <= stack->val.op.p && tok->val.op.a == l_to_r)
        || (tok->val.op.p < stack->val.op.p && tok->val.op.a == r_to_l)) {
          if (stack_pop(&stack, &tmp) != OK){
            LOG_ERROR("\n>> Err: Something went wrong, %s:%d", __FILE__, __LINE__);
            free(tok);
            return ERROR;
          }
          enqueue(queue,tmp);
        } else {
          break;
        }
      }
      stack_push(&stack, tok);
    }
  } while (**ex != '\0');

  token_t * tmp;
  while (stack_pop(&stack, &tmp) == OK){
    enqueue(queue, tmp);
  }
  return OK;
}

void log_rpn(token_queue_t *queue) {
  if (queue->head == NULL) {
    LOG_INFO("\n>> got null token queue");
    return;
  }
  token_t *tok = queue->head;
  LOG_INFO("\n>> RPN: ");
  while (tok != NULL) {
    if (tok->type == OPERAND) {
      LOG_INFO("%lf ", tok->val.num);
    } else if (tok->type == OPERATOR) {
      LOG_INFO("%d ", tok->val.op.s);
    } else if (tok->type == UNARY_OP) {
      LOG_INFO("%d ", tok->val.op.s);
    }
    tok = tok->next;
  }
}
