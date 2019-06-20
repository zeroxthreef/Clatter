#ifndef UTIL_H__
#define UTIL_H__

#include "types.h"

#include <stdint.h>
#include <stddef.h>

short tl_asprintf(char **string, const char *fmt, ...);

short is_whitespace_utf8(void *character);

void clat_skipto_utf8(void **source, unsigned long position);

void clat_print_tokens(clat_token_t *tokens, unsigned long length);

void clat_print_repetitive(clat_ctx_t *ctx, char ch, int num);

int clat_determine_if_number(clat_ctx_t *ctx, void *value);

#endif