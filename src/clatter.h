#ifndef CLATTER_H__
#define CLATTER_H__

#include "util.h"
#include "lexer.h"
#include "parser.h"

int clat_compile_string(clat_ctx_t *ctx, char *source, uint32_t flags);

clat_val_t clat_eval_string(clat_ctx_t *ctx, char *source);

int clat_cleanup(clat_ctx_t *ctx);

int clat_add_function(clat_ctx_t *ctx, char *atom, clat_val_t(*clat_callback)(clat_ctx_t *ctx, clat_val_t *arguments, uint16_t argument_num));

#endif