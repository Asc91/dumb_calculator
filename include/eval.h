#ifndef _EVAL_H_
#define _EVAL_H_

#include "../include/token.h"

err_t eval_op(double left, double right, operator_t op, token_val_t *result);

err_t eval_rpn(token_queue_t *queue, token_t *result);

err_t eval_expression(char *expression, token_t *result);
#endif
