#include "clatter.h"

#include "util.h"
#include "lexer.h"
#include "parser.h"

#include "stdlib/clat_stdlib.h"
#include "utf8.h"

#include <string.h>
#include <stdlib.h>

/* internal utils */

/* handle both symbols and identifiers */
static void clat_internal_destroy(void *row_struct)
{
	clat_table_row_t *row = row_struct;

	/* TODO handle this better */
	/*
	if((*(clat_val_t *)row->value).value)
		free((*(clat_val_t *)row->value).value);
	free(row->value);
	*/
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

	value = clat_execute_ast(ctx, ctx->root, NULL, value);

	return value;
}

clat_val_t clat_execute_ast(clat_ctx_t *ctx, clat_ast_node_t *ast, clat_ast_node_t *next, clat_val_t last_value)
{
	unsigned long i, argument_num = 0;
	clat_val_t value, *arguments = NULL;
	clat_ast_node_t *temp = NULL;
	clat_var_t variable;
	clat_object_t object;
	clat_table_row_t *row = NULL;
	uint8_t arg_flags = 0;
	value.type = CLAT_TYPE_NONE;
	value.value = NULL;

	/* NOTE if the TABLE TYPE IS CLAT_TABLE_TYPE_STATIC_VARIABLE, THEN DO NOOOOT FREE VALUE. This is how static vars retain their data */

	/* first, if we've entered a block, that means we need to add the function symbols to the symbol list */
	if(ast->type == CLAT_NODE_BLOCK)
	{
		for(i = 0; i < ((clat_ast_node_block_t *)ast->data)->function_num; i++)
		{
			memset(&variable, 0, sizeof(clat_var_t));
			memset(&object, 0, sizeof(clat_object_t));

			/* add object to list and also to the symbol table */

			object.value.value = ((clat_ast_node_block_t *)ast->data)->functions[i].ast_node;
			object.value.type = CLAT_TYPE_FUNCTION;
			//clat_reference_count_inc(ctx, ctx->objects.object_num);
			object.references = 1;


			if(clat_add_array_entry(&ctx->objects.objects, ctx->objects.object_num, &object, sizeof(clat_object_t)))
			{
				/* TODO handle error */
				return value;
			}

			variable.symbol = ((clat_ast_node_block_t *)ast->data)->functions[i].symbol;
			variable.identifier = ((clat_ast_node_block_t *)ast->data)->functions[i].identifier;


			if(clat_table_add_row(ctx->locals, CLAT_TABLE_TYPE_FUNCTION, clat_memdup(&variable.symbol, sizeof(uint32_t)), NULL))
			{
				/* TODO handle error */
				return value;
			}

			if(clat_table_add_row(ctx->symbols, CLAT_TABLE_TYPE_FUNCTION, clat_memdup(&variable.symbol, sizeof(uint32_t)), clat_memdup(&variable, sizeof(clat_var_t))))
			{
				/* TODO handle error */
				return value;
			}

			ctx->objects.object_num++;
		}
	}
	else if(ast->type == CLAT_NODE_FUNCTION_CALL)
	{
		/* check if it asks for previous val and/or next block */
		/* for c functions check for flags, for clatter functions check for __PREVRES and __BLK */
		if(!(row = clat_table_row_at(ctx->symbols, &((clat_ast_node_func_call_t *)ast->data)->symbol)))
		{
			/* TODO handle error */
			printf("CRITICAL SYMBOL TABLE ERROR\n");
			return value;
		}
		


		if(row->type == CLAT_TABLE_TYPE_CALLBACK)
		{
			arg_flags = ((clat_callback_t *)row->value)->flags;
			argument_num = ast->num_children + clat_read_bitflag(arg_flags, CLAT_CALLBACK_WANT_LAST_RETURN) + clat_read_bitflag(arg_flags, CLAT_CALLBACK_WANT_NEXT_BLOCK);

			arguments = malloc(argument_num * sizeof(clat_val_t));
/* ============================================================================================================================================ */
			/* TODO handle block types */
			//arguments[argument_num - 1] = ; next_value
		}
		else if(row->type == CLAT_TABLE_TYPE_FUNCTION)
		{
			/* check for the aformentioned 2 cases of special arguments */
			for(i = 0; i < ((clat_ast_node_t *)row->value)->num_children - 1; i++)
			{
				if(((clat_ast_node_t *)row->value)->children[i].type != CLAT_NODE_ATOM_LITERAL)
				{
					/* TODO handle error */
					return value;
				}

				if(utf8cmp(((clat_ast_node_t *)row->value)->children[i].data, "__PREVRES") == 0)
					arg_flags |= CLAT_CALLBACK_WANT_LAST_RETURN;
				else if(utf8cmp(((clat_ast_node_t *)row->value)->children[i].data, "__BLK") == 0)
					arg_flags |= CLAT_CALLBACK_WANT_NEXT_BLOCK;
			}
		}
	}

	for(i = 0; i < ast->num_children; i++)
	{
		/* only care about getting return values from calls, literals, and blocks */

		/* test if its adding arguments to a function call and add variables of the same name to the
		symbol table */
		//if(ast->children[i].type == CLAT_NODE_FUNCTION_CALL)
		{
			//value = clat_execute_ast(ctx, &ast->children[i], i + 1 < ast->num_children ? &ast->children[i + 1] : NULL, last_value);
		}
		/*else*/ if(ast->children[i].type != CLAT_NODE_FUNCTION_DEFINITION)
			value = clat_execute_ast(ctx, &ast->children[i], i + 1 < ast->num_children ? &ast->children[i + 1] : NULL, last_value);
		
		if(ast->type == CLAT_NODE_FUNCTION_CALL)
		{
			/* look at the definition and make new vars */
			/* immediately copy the value if the prevres flag is enabled */

			/* we should already have the row pointer */
			/* also, we should NOT make new local variables for the function called if its a c callback */
			if(row->type == CLAT_TABLE_TYPE_CALLBACK)
			{
				/* offset because the first is always prevres if asked, and last is always blk if asked */

				if(clat_read_bitflag(arg_flags, CLAT_CALLBACK_WANT_LAST_RETURN))
					arguments[i + 1] = value;
				else
					arguments[i] = value;
			}
			else if(row->type == CLAT_TABLE_TYPE_FUNCTION)
			{
				/* TODO add vars to the stack symbol table*/
				
			}

		}
	}


	switch(ast->type)
	{
		case CLAT_NODE_NUMBER_LITERAL:
			value = clat_double_to_value(ctx, *(double *)ast->data);
		break;
		case CLAT_NODE_STRING_LITERAL:
			value = clat_string_to_value(ctx, ast->data);
		break;
		case CLAT_NODE_ATOM_LITERAL:
			value = clat_string_to_value(ctx, ast->data);
		break;
		/* TODO handle references */
		case CLAT_NODE_FUNCTION_CALL:
			if(row->type == CLAT_TABLE_TYPE_CALLBACK)
			{
				value = ((clat_callback_t *)row->value)->clat_callback(ctx, arguments, argument_num);
				/* TODO clean up the arguments */
			}
			else if(row->type == CLAT_TABLE_TYPE_FUNCTION)
			{
				printf("oof\n");
				/* handle creating and estroying argument vars */
				/* NOTE: a "..." var will be added if argument count goes over whats specified in the definition.
				This will be an array of values */
			}
		break;
	}

	/* clear locally added variables */
printf("TEMPORARY TEST %f\n", value.type == CLAT_TYPE_NUMBER ? clat_value_to_double(ctx, value) : 1.1);

	return value;
}

int clat_initialize(clat_ctx_t *ctx)
{

	memset(ctx, 0, sizeof(clat_ctx_t));
	memset(&ctx->objects, 0, sizeof(clat_object_list_t));


	if(clat_table_init(&ctx->symbols, &clat_internal_symbol_compare, &clat_internal_destroy))
	{
		/* TODO handle error */
		return 1;
	}

	if(clat_table_init(&ctx->locals, &clat_internal_symbol_compare, &clat_internal_destroy))
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
	clat_table_destroy(ctx->locals);

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