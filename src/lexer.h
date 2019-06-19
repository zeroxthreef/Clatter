#ifndef LEXER_H__
#define LEXER_H__

#include "util.h"
#include "types.h"

clat_token_t *clat_lex_string(clat_ctx_t *ctx, char *source, unsigned long *length);

void clat_lex_clean(clat_ctx_t *ctx, clat_token_t *tokens, unsigned long tokens_length);

#endif