#ifndef _EVAL_H_
#define _EVAL_H_

#include "../include/token.h"
token_val_t eval_op(double left, double right, operator_t op);

token_t *eval_rpn(token_queue_t *queue);
#endif
