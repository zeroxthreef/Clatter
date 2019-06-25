#ifndef UTIL_H__
#define UTIL_H__

#include "types.h"

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>

short tl_asprintf(char **string, const char *fmt, ...);

short is_whitespace_utf8(void *character);

void clat_skipto_utf8(void **source, unsigned long position);

void clat_print_tokens(clat_token_t *tokens, unsigned long length);

void clat_print_repetitive(clat_ctx_t *ctx, char ch, int num);

int clat_determine_if_number(clat_ctx_t *ctx, void *value);

uint8_t *clat_read_file(clat_ctx_t *ctx, size_t begin, size_t end, const char *path, size_t *size);

ssize_t clat_write_file(clat_ctx_t *ctx, uint8_t data, const char *path, size_t size);

short clat_read_bitflag(uint64_t value, short bit);

int clat_realloc_array(void **ptr, size_t currentNum, size_t wantNum, size_t sizeOfIndividualObject);

int clat_remove_array_entry(void **ptr, size_t currentNum, size_t positionToRemove, size_t sizeOfIndividualObject);

int clat_add_array_entry(void **ptr, size_t currentNum, void *objectPtr, size_t sizeOfIndividualObject);

int clat_table_init(clat_table_t **table, uint8_t (*compare)(uint8_t type, void *key, void *test), void (*destroy)(void *row_struct));

int clat_table_destroy(clat_table_t *table);

int clat_table_add_row(clat_table_t *table, uint8_t type, void *key, void *value);

int clat_table_remove_row(clat_table_t *table, unsigned long row);

void *clat_table_value_at(clat_table_t *table, void *key, unsigned long *position);

#endif