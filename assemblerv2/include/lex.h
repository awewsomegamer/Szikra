#ifndef LEX_H
#define LEX_H

#include <global.h>

extern uint8_t _eof_reached;

int next_token(struct token* t);

#endif