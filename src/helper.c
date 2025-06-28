#include "../include/helper.h"
#include "../include/token.h"
// Helper functions
void flush_stdin() {
  int ch;
  do {
    ch = getchar();
  } while (ch != '\n' && ch != EOF);
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
      LOG_INFO("%d ", tok->val.op.p);
    } else if (tok->type == UNARY_OP) {
      LOG_INFO("%d ", tok->val.op.s);
    }
    tok = tok->next;
  }
}
