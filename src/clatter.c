#include "clatter.h"

#include "util.h"
#include "lexer.h"
#include "parser.h"

int clat_compile_string(clat_ctx_t *ctx, char *source, uint32_t flags)
{
	return clat_parse_string(ctx, source, flags);
}

clat_val_t clat_eval_string(clat_ctx_t *ctx, char *source)
{
	clat_val_t value;
	value.type = CLAT_TYPE_NUMBER;
	value.value = NULL;

	if(clat_parse_string(ctx, source, 0))
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

int clat_cleanup(clat_ctx_t *ctx)
{
	/* clean up the AST */
	return 0;
}

int clat_add_function(clat_ctx_t *ctx, char *atom, clat_val_t(*clat_callback)(clat_ctx_t *ctx, clat_val_t *arguments, uint16_t argument_num))
{

	return 0;
}