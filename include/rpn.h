#ifndef _RPN_H_
#define _RPN_H_

#include "../include/token.h"

err_t create_rpn(expression_t *ex, token_queue_t *queue);

void log_rpn(token_queue_t *queue);

#endif