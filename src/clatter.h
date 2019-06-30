#ifndef CLATTER_H__
#define CLATTER_H__

#include "util.h"
#include "lexer.h"
#include "parser.h"

int clat_compile_string(clat_ctx_t *ctx, char *source, uint32_t flags, uint8_t stdlib_level);

clat_val_t clat_eval_string(clat_ctx_t *ctx, char *source);

clat_val_t clat_execute_ast(clat_ctx_t *ctx, clat_ast_node_t *ast, clat_ast_node_t *next, clat_val_t last_value);

int clat_initialize(clat_ctx_t *ctx);

int clat_cleanup(clat_ctx_t *ctx);

int clat_add_function(clat_ctx_t *ctx, char *atom, clat_val_t(*clat_callback)(clat_ctx_t *ctx, clat_val_t *arguments, uint16_t argument_num), uint8_t flags);

#endif