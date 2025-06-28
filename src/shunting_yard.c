#include "../include/shunting_yard.h"
#include "../include/token.h"
#include <stdlib.h>
#include <stdio.h>

token_queue_t *shunting_yard(expression_t *ex) {
  if (ex == NULL) {
    printf("\n>> got null expression");
    return NULL;
  }
  token_stack_t stack = NULL;
  token_queue_t *queue = malloc(sizeof(token_queue_t));
  init_token_queue(queue);
  int index = 0;
  do {
    token_t *tok = tokanizer(ex, &index);
    if (tok == NULL) {
      continue;
    }
    if (tok->type == OPERAND) {
      enqueue(queue, tok);
    } else if (tok->type == OPERATOR || tok->type == UNARY_OP) {

      while (stack != NULL && tok->val.op.p < stack->val.op.p) {
        enqueue(queue, stack_pop(&stack));
      }

      stack_push(&stack, tok);
    }
  } while (**ex != '\0');

  while (stack != NULL) {
    enqueue(queue, stack_pop(&stack));
  }
  return queue;
}
