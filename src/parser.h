#ifndef PARSER_H__
#define PARSER_H__

#include "types.h"

int clat_parse_string(clat_ctx_t *ctx, char *source, uint32_t flags);

int clat_parse_init_ast(clat_ctx_t *ctx, clat_ast_node_t **ast);

unsigned long clat_parse_generate_ast(clat_ctx_t *ctx, clat_token_t *tokens, clat_ast_node_t *parent, unsigned long token_num, unsigned long token_pos);

int clat_parse_finalize_ast(clat_ctx_t *ctx, clat_ast_node_t *ast);

void clat_parse_print(clat_ctx_t *ctx, clat_ast_node_t *ast);

#endif