#include "parser.h"

#include "types.h"
#include "lexer.h"
#include "util.h"

#include "utf8.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int clat_parse_string(clat_ctx_t *ctx, char *source, uint32_t flags)
{
	clat_token_t *tokens = NULL;
	unsigned long tokens_length = 0;

	if(!(tokens = clat_lex_string(ctx, source, &tokens_length)))
	{
		/* handle error */
		return 1;
	}

	if(clat_read_bitflag(flags, 1))
		clat_print_tokens(tokens, tokens_length);

	/* ast-ify the tokens */
	/* NOTE the current "block" needs to be scanned for functions all the way */


	/* check if the ast is null as a test of whether or not to add utils */
	if(!ctx->root)
		if(clat_parse_init_ast(ctx, &ctx->root))
		{
			/* handle error */
			return 1;
		}

	/* NOTE NOTE function calls will have child nodes if theres a block expression below */
	/* function def:  [atom] [(] (REF)ATOMS [)] [:] [{] EXPRESSIONS [}] */
	/* function call: [atom] [(] ATOMS*+EXPRESSIONS [)] */
	/* special arg atoms for func definitions:
											- "..." is an array of arguments that arent explicitly caught

											- "__PREVRES" feeds the last expressions result into the function.
											a VERY important note is that it only saves the previous result if
											the function call used a block call. If anything comes next, it
											is immediately forgotten

											- "__BLK" is a special variable that acts like a function, but
											its really used to execute the block following the function.
											You can also pass a single variable to it optionally. This makes
											the variable __var appear inside the block run
	
	EXAMPLE: notif(__PREVRES condition _BLK):{if(!(condition)){_BLK()return(1)}else(){return(0)}}
	*/


	if(clat_parse_generate_ast(ctx, tokens, ctx->root, tokens_length, 0) == -1)
	{
		/* handle error */
		printf("big error\n");
		return 1;
	}

	//if(clat_parse_finalize_ast(ctx, ctx->root))
	{
		/* handle error */
		//return 1;
	}


	/* cleanup the tokens because theyre no longer needed */
	clat_lex_clean(ctx, tokens, tokens_length);
	return 0;
}

int clat_parse_init_ast(clat_ctx_t *ctx, clat_ast_node_t **ast)
{
	if(!(*ast = calloc(1, sizeof(clat_ast_node_t))))
	{
		/* handle error */
		return 1;
	}

	ctx->root->type = CLAT_TYPE_BLOCK;
	ctx->root->children = NULL;
	ctx->root->data = NULL;
	ctx->root->num_children = 0;

	/* add default functions */


	return 0;
}

unsigned long clat_parse_generate_ast(clat_ctx_t *ctx, clat_token_t *tokens, clat_ast_node_t *parent, unsigned long token_num, unsigned long token_pos)
{
	unsigned long i, new_pos = 0;
	double temp_num;

	for(i = token_pos; i < token_num; i++)
	{
		/* test for larger possibilities first like function definitions, then calls, then blocks */

		/* sort of hacky, but theres no way to communicate with the parent when inside the block,
		so when we're inside the parent and want to escape after the one and only block is
		allowed, we exit */

		if(parent->type == CLAT_NODE_FUNCTION_DEFINITION)
		{
			//if(parent->num_children > 0 && parent->children[parent->num_children - 1].type == CLAT_NODE_BLOCK)
				//break;
		}


		if(tokens[i].token == CLAT_TOK_ATOM)
		{
			/* its either a call or a definition, so we'll just assume its both until a colon is encountered or 
			anything but plain atoms are inside the parentheses */

			if(i + 1 != token_num && tokens[i + 1].token == CLAT_TOK_PARENTH_OPEN)
			{
				/* NODE, if at any point, the parent turns from CLAT_NODE_FUNCTION_INDETERMINATE to a type, it needs to stay that way */
				if(!(parent->children = realloc(parent->children, sizeof(clat_ast_node_t) * (parent->num_children + 1))))
				{
					/* handle error */
					return -1;
				}

				memset(&parent->children[parent->num_children], 0, sizeof(clat_ast_node_t));
				
				parent->children[parent->num_children].type = CLAT_NODE_FUNCTION_INDETERMINATE;

				/* store the atom as the data */
				if(!(parent->children[parent->num_children].data = utf8dup(tokens[i].data)))
				{
					/* handle error */
					return -1;
				}

				if((new_pos = clat_parse_generate_ast(ctx, tokens, &parent->children[parent->num_children], token_num, i + 1)) == -1)
				{
					/* handle error */
					return -1;
				}

				parent->num_children++;

				i = new_pos;
			}
			else /* its a regular atom in an argument or argument var names in a header */
			{
				if(!(parent->children = realloc(parent->children, sizeof(clat_ast_node_t) * (parent->num_children + 1))))
				{
					/* handle error */
					return -1;
				}

				memset(&parent->children[parent->num_children], 0, sizeof(clat_ast_node_t));

				/* determine if the atom is a number, or plan atom. STRING LITERALS ARE HANDLED SEPEARATELY */

				parent->children[parent->num_children].type = clat_determine_if_number(ctx, tokens[i].data) ? CLAT_NODE_NUMBER_LITERAL : CLAT_NODE_ATOM_LITERAL;

				if(parent->children[parent->num_children].type == CLAT_NODE_ATOM_LITERAL)
					parent->children[parent->num_children].data = utf8dup(tokens[i].data);
				else
				{
					if(!(parent->children[parent->num_children].data = calloc(1, sizeof(double))))
					{
						/* handle error */
						return 1;
					}
					
					temp_num = strtod(tokens[i].data, NULL);
					(*(double *)parent->children[parent->num_children].data) = temp_num;
				}

				parent->num_children++;
			}

		}
		else if(tokens[i].token == CLAT_TOK_BRACE_OPEN) /* NOTE this is not called when a block follows some function. THose are part of function definitions */
		{
			/* its an expression block */
			if(!(parent->children = realloc(parent->children, sizeof(clat_ast_node_t) * (parent->num_children + 1))))
			{
				/* handle error */
				return -1;
			}
			
			memset(&parent->children[parent->num_children], 0, sizeof(clat_ast_node_t));
			
			parent->children[parent->num_children].type = CLAT_NODE_BLOCK;
			if(!(parent->children[parent->num_children].data = calloc(1, sizeof(clat_ast_node_block_t))))
			{
				/* handle error */
				return -1;
			}

			if((new_pos = clat_parse_generate_ast(ctx, tokens, &parent->children[parent->num_children], token_num, i + 1)) == -1)
			{
				/* handle error */
				return -1;
			}

			parent->num_children++;

			i = new_pos;
		}
		else if(tokens[i].token == CLAT_TOK_STRING)
		{
			if(!(parent->children = realloc(parent->children, sizeof(clat_ast_node_t) * (parent->num_children + 1))))
			{
				/* handle error */
				return -1;
			}
			
			memset(&parent->children[parent->num_children], 0, sizeof(clat_ast_node_t));
			
			parent->children[parent->num_children].type = CLAT_NODE_STRING_LITERAL;
			parent->children[parent->num_children].data = utf8dup(tokens[i].data);

			parent->num_children++;
		}
		else if(tokens[i].token == CLAT_TOK_PARENTH_CLOSE)
		{
			/* check if theres a colon following */
			if(i + 1 != token_num && tokens[i + 1].token == CLAT_TOK_COLON) /* its a definition. Dont exit yet */
			{
				if(parent->type == CLAT_NODE_FUNCTION_INDETERMINATE)
					parent->type = CLAT_NODE_FUNCTION_DEFINITION;
			}
			else /* its a call, so we can just exit this node */
			{
				if(parent->type == CLAT_NODE_FUNCTION_INDETERMINATE)
					parent->type = CLAT_NODE_FUNCTION_CALL;

				new_pos = i;
				break;
			}
		}
		else if(tokens[i].token == CLAT_TOK_AMPERSAND)
		{
			if(i + 1 != token_num && tokens[i + 1].token == CLAT_TOK_ATOM)
			{
				if(!(parent->children = realloc(parent->children, sizeof(clat_ast_node_t) * (parent->num_children + 1))))
				{
					/* handle error */
					return -1;
				}
				
				memset(&parent->children[parent->num_children], 0, sizeof(clat_ast_node_t));
				
				parent->children[parent->num_children].type = CLAT_NODE_REFERENCE_ATOM_LITERAL;
				parent->children[parent->num_children].data = utf8dup(tokens[i + 1].data);

				parent->num_children++;

				i++;
			}
		}
		else if(tokens[i].token == CLAT_TOK_BRACE_CLOSE)
		{
			new_pos = i;
			break;
		}
	}


	return new_pos;
}

/* this _has_ to be run even if someone didnt want a refined ast because it throws function definitions into the parent node */
int clat_parse_finalize_ast(clat_ctx_t *ctx, clat_ast_node_t *ast)
{
	unsigned long i, j;
	clat_ast_node_t *temp = ast;

	/* optimize * and + */
	/* TODO / and -, but they have to be consecutive */
	/*
	if(temp->type == CLAT_NODE_ATOM_LITERAL && utf8cmp(temp->data, "+") == 0)
	{
		for(i = 0; i < ast->num_children; i++)
		{
			temp = &ast->children[i];
			if(temp->type == CLAT_NODE_NUMBER_LITERAL)
			{
				/* combine number literals *//*
			}
		}

		temp = ast;
	}
	*/

	for(i = 0; i < ast->num_children; i++)
	{
		temp = &ast->children[i];

		/* add the function identifiers to a list inside the block */
		if(temp->type == CLAT_NODE_BLOCK)
		{
			for(j = 0; j < temp->num_children; j++)
			{
				if(temp->children[i].type == CLAT_NODE_FUNCTION_DEFINITION)
				{
					((clat_ast_node_block_t *)temp->data)->functions = realloc(((clat_ast_node_block_t *)temp->data)->functions, ((clat_ast_node_block_t *)temp->data)->function_num + 1);

					((clat_ast_node_block_t *)temp->data)->functions[((clat_ast_node_block_t *)temp->data)->function_num].identifier = temp->children[i].data;
					((clat_ast_node_block_t *)temp->data)->functions[((clat_ast_node_block_t *)temp->data)->function_num].ast_node = &temp->children[i];

					((clat_ast_node_block_t *)temp->data)->function_num++;
				}
			}
		}

		if(clat_parse_finalize_ast(ctx, temp))
		{
			return 1;
		}
	}
	return 0;
}

static void clat_parse_print_internal(clat_ctx_t *ctx, clat_ast_node_t *ast, unsigned long level)
{
	unsigned long i, j;
	clat_ast_node_t *temp = NULL;

	for(i = 0; i < ast->num_children; i++)
	{
		temp = &ast->children[i];

		clat_print_repetitive(ctx, '\t', level);
		switch(temp->type)
		{
			case CLAT_NODE_BLOCK:
				printf("block:\n");
				/*clat_print_repetitive(ctx, '\t', level);
				printf("definitions[\n");
				for(j = 0; j < ((clat_ast_node_block_t *)temp->data)->function_num; j++)
				{
					clat_print_repetitive(ctx, '\t', level);
					printf("%s\n", ((clat_ast_node_block_t *)temp->data)->functions[j].identifier);
				}
				clat_print_repetitive(ctx, '\t', level);
				printf("]\n"); */
			break;
			case CLAT_NODE_ATOM_LITERAL:
				printf("atm:%s\n", temp->data);
			break;
			case CLAT_NODE_NUMBER_LITERAL:
				printf("num:%lf\n", *(double *)temp->data);
			break;
			case CLAT_NODE_STRING_LITERAL:
				printf("str:%s\n", temp->data);
			break;
			case CLAT_NODE_FUNCTION_CALL:
				printf("cal:%s\n", temp->data);
			break;
			case CLAT_NODE_FUNCTION_DEFINITION:
				printf("dec:%s\n", temp->data);
			break;
			case CLAT_NODE_REFERENCE_ATOM_LITERAL:
				printf("ref:%s\n", temp->data);
			break;
			default:
				printf("?\n");
		}

		clat_parse_print_internal(ctx, temp, level + 1);
	}
}

void clat_parse_print(clat_ctx_t *ctx, clat_ast_node_t *ast)
{
	clat_parse_print_internal(ctx, ast, 0);
}