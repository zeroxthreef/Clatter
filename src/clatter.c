#include "clatter.h"

#include "util.h"
#include "lexer.h"
#include "parser.h"

#include "stdlib/clat_stdlib.h"

#include <string.h>
#include <stdlib.h>

/* internal utils */

/* handle both symbols and identifiers */
static void clat_internal_destroy(void *row_struct)
{
	clat_table_row_t *row = row_struct;

	free(row->value);
	free(row->key);
}

static uint8_t clat_internal_symbol_compare(uint8_t type, void *key, void *test)
{
	if(*(uint32_t *)test == *(uint32_t *)key)
		return 1;
	return 0;
}


/* regular functions */


/* NOTE: the easy interface functions dont provide an stdlib level because if you want finer control,
call the parse and execute functions separately */
int clat_compile_string(clat_ctx_t *ctx, char *source, uint32_t flags, uint8_t stdlib_level)
{

	if(clat_parse_string(ctx, &ctx->root, source, flags))
	{
		/* TODO handle error */
		return 1;
	}
	
	if(clat_add_stdlib(ctx, stdlib_level))
	{
		/* TODO handle error */
		return 1;
	}
	
	
	return 0;
}

clat_val_t clat_eval_string(clat_ctx_t *ctx, char *source)
{
	clat_val_t value;
	value.type = CLAT_TYPE_NUMBER;
	value.value = NULL;

	if(clat_compile_string(ctx, source, 0, CLAT_STDLIB_FULL))
	{
		/* handle error */
		return value;
	}

	/* now execute */

	value = clat_execute_ast(ctx, ctx->root);

	return value;
}

clat_val_t clat_execute_ast(clat_ctx_t *ctx, clat_ast_node_t *ast)
{
	clat_val_t value;
	value.type = CLAT_TYPE_NUMBER;
	value.value = NULL;

	




	return value;
}

int clat_initialize(clat_ctx_t *ctx)
{

	memset(ctx, 0, sizeof(clat_ctx_t));


	if(clat_table_init(&ctx->symbols, &clat_internal_symbol_compare, &clat_internal_destroy))
	{
		/* TODO handle error */
		return 1;
	}


	return 0;
}

int clat_cleanup(clat_ctx_t *ctx)
{
	/* clean up the AST and other allocated contents */

	clat_table_destroy(ctx->symbols);

	return 0;
}

int clat_add_function(clat_ctx_t *ctx, char *atom, clat_val_t(*clat_callback)(clat_ctx_t *ctx, clat_val_t *arguments, uint16_t argument_num), uint8_t flags)
{
	clat_callback_t *callback = calloc(1, sizeof(clat_callback_t));

	if(!callback)
	{
		/* handle error */
		return 1;
	}

	callback->clat_callback = clat_callback;
	callback->flags = flags;

	if(clat_table_add_row_hash(ctx->symbols, CLAT_TABLE_TYPE_CALLBACK, atom, callback))
	{
		/* handle error */
		return 1;
	}

	return 0;
}