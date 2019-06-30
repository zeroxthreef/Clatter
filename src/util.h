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

short clat_read_bitflag(uint64_t value, uint64_t flag);

short clat_read_bitindex(uint64_t value, short bit);

int clat_realloc_array(void **ptr, size_t currentNum, size_t wantNum, size_t sizeOfIndividualObject);

int clat_remove_array_entry(void **ptr, size_t currentNum, size_t positionToRemove, size_t sizeOfIndividualObject);

int clat_add_array_entry(void **ptr, size_t currentNum, void *objectPtr, size_t sizeOfIndividualObject);

void *clat_memdup(void *src, size_t size);

uint64_t clat_hash(char *string);

int clat_table_init(clat_table_t **table, uint8_t (*compare)(uint8_t type, void *key, void *test), void (*destroy)(void *row_struct));

int clat_table_destroy(clat_table_t *table);

int clat_table_clear(clat_table_t *table);

int clat_table_add_row_hash(clat_table_t *table, uint8_t type, char *key, void *value);

int clat_table_add_row(clat_table_t *table, uint8_t type, void *key, void *value);

int clat_table_remove_row(clat_table_t *table, unsigned long row);

void *clat_table_value_at(clat_table_t *table, void *key, unsigned long *position);

clat_table_row_t *clat_table_row_at(clat_table_t *table, void *key);

void *clat_table_value_at_hash(clat_table_t *table, char *key, unsigned long *position);

uint8_t clat_table_default_hash_compare(uint8_t type, void *key, void *test);

int clat_destroy_value(clat_ctx_t *ctx, clat_val_t value);

clat_val_t clat_double_to_value(clat_ctx_t *ctx, double value);

double clat_value_to_double(clat_ctx_t *ctx, clat_val_t value);

clat_val_t clat_string_to_value(clat_ctx_t *ctx, char *value);

char *clat_value_to_string(clat_ctx_t *ctx, clat_val_t value);

int clat_reference_count_inc(clat_ctx_t *ctx, unsigned long position);

int clat_reference_count_dec(clat_ctx_t *ctx, unsigned long position);

#endif