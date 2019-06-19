#include "lexer.h"

#include "utf8.h"

#include <stdlib.h>
#include <string.h>

static void *clat_lex_strlit(clat_ctx_t *ctx, void *source, unsigned long *end)
{
	/* iterate codepoint by codepoint and un-escape + test for string literal end */
	short in_string = 0, in_escape = 0;
	void *source_pos = source, *string = NULL;
	unsigned long source_length = utf8len(source), i;
	utf8_int32_t value = 0;
	/* =========================== */

	/* initialize an empty string */
	string = calloc(1, sizeof(uint8_t));

	for(i = 0; i < source_length; i++)
	{
		source_pos = utf8codepoint(source_pos, &value);

		if(value == '"' && !in_escape)
			break;


		switch(value)
		{
			case '\\':
				if(!in_escape)
				{
					in_escape++;
					value = 0x00;
				}
				else
				{
					value = '\\';
					in_escape = 0;
				}
			break;
			case '"':
				/* dont need to test if in escape because if it isnt, it'd've already escaped */
				value = '\"';
				in_escape = 0;
			break;
			case 'n':
				if(in_escape)
				{
					value = '\n';
					in_escape = 0;
				}
			break;
			case 'r':
				if(in_escape)
				{
					value = '\r';
					in_escape = 0;
				}
			break;
			case 'a':
				if(in_escape)
				{
					value = '\a';
					in_escape = 0;
				}
			break;
			case 'b':
				if(in_escape)
				{
					value = '\b';
					in_escape = 0;
				}
			break;
			case 'f':
				if(in_escape)
				{
					value = '\f';
					in_escape = 0;
				}
			break;
			case 't':
				if(in_escape)
				{
					value = '\t';
					in_escape = 0;
				}
			break;
			case 'v':
				if(in_escape)
				{
					value = '\v';
					in_escape = 0;
				}
			break;
		}

		if(value)
		{
			string = realloc(string, utf8size(string) + utf8codepointsize(value));
			utf8ncat(string, &value, 1);
		}

	}
	*end = i + 1;
	
	return string;
}

static int clat_lex_add_token(clat_ctx_t *ctx, clat_token_t **tokens, unsigned long *token_count, uint8_t token, void *data, uint32_t line)
{
	if(!*tokens)
	{
		if(!(*tokens = calloc(1, sizeof(clat_token_t))))
		{
			/* express error */
			printf("TEMP ERROR %s\n", "bad calloc");
			return 1;
		}
	}
	else
	{
		if(!(*tokens = realloc(*tokens, (*token_count + 1) * sizeof(clat_token_t)) ))
		{
			/* express error */
			printf("TEMP ERROR %s\n", "bad realloc");
			return 1;
		}
	}

	/* add token */

	(*tokens)[*token_count].token = token;
	(*tokens)[*token_count].data = data;
	#ifdef REMEMBER_LINES
	(*tokens)[*token_count]->line = line;
	#endif

	(*token_count)++;
	return 0;
}

clat_token_t *clat_lex_string(clat_ctx_t *ctx, char *source, unsigned long *length)
{
	short claimed = 0;
	uint8_t token;
	clat_token_t *tokens = NULL, previous_token;
	void *source_pos = source, *current_atom = NULL;
	unsigned long tokens_length = 0, source_length = utf8len(source_pos), line = 0, i, skipto = 0;
	utf8_int32_t value = 0;

	/* =========================== */

	for(i = 0; i < source_length; i++)
	{
		source_pos = utf8codepoint(source_pos, &value);

		switch(value)
		{
			case '(':
				claimed = 1;
				token = CLAT_TOK_PARENTH_OPEN;
			break;
			case ')':
				claimed = 1;
				token = CLAT_TOK_PARENTH_CLOSE;
			break;
			case '{':
				claimed = 1;
				token = CLAT_TOK_BRACE_OPEN;
			break;
			case '}':
				claimed = 1;
				token = CLAT_TOK_BRACE_CLOSE;
			break;
			case '&':
				claimed = 1;
				token = CLAT_TOK_AMPERSAND;
			break;
			case ':':
				claimed = 1;
				token = CLAT_TOK_COLON;
			break;
			case '"':
				claimed = 1;
				/* first, add the current atom if there was one in progress */
				if(current_atom)
					clat_lex_add_token(ctx, &tokens, &tokens_length, CLAT_TOK_ATOM, current_atom, line);

				/* add the string */
				current_atom = clat_lex_strlit(ctx, source_pos, &skipto);
				clat_skipto_utf8(&source_pos, skipto);
				i+= skipto;

				clat_lex_add_token(ctx, &tokens, &tokens_length, CLAT_TOK_STRING, current_atom, line);

				current_atom = NULL;
			break;
			#ifdef REMEMBER_LINES
			case '\n':
				line++;
			break;
			#endif
		}

		if(!claimed)
		{
			if(!is_whitespace_utf8(&value) && !current_atom)
			{
				current_atom = calloc(1, utf8codepointsize(value) + 1);
				utf8ncat(current_atom, &value, 1);
			}
			else if(!is_whitespace_utf8(&value) && current_atom)
			{
				current_atom = realloc(current_atom, utf8size(current_atom) + utf8codepointsize(value));
				utf8ncat(current_atom, &value, 1);
			}
			else if(is_whitespace_utf8(&value) && current_atom)
			{
				clat_lex_add_token(ctx, &tokens, &tokens_length, CLAT_TOK_ATOM, current_atom, line);
				current_atom = NULL;
			}
		}
		else if(claimed && current_atom)
		{
			/* have to commit 2 tokens to the list */
			clat_lex_add_token(ctx, &tokens, &tokens_length, CLAT_TOK_ATOM, current_atom, line);
			current_atom = NULL;

			clat_lex_add_token(ctx, &tokens, &tokens_length, token, NULL, line);
			
		}
		else if(claimed && value != '"')
			clat_lex_add_token(ctx, &tokens, &tokens_length, token, NULL, line);

//printf("TEMP_LEX_PROGRESS %c %lu\n", value, utf8codepointsize(value));
		claimed = 0;
	}
	

	/* =========================== */
	
	*length = tokens_length;

	return tokens;
}

void clat_lex_clean(clat_ctx_t *ctx, clat_token_t *tokens, unsigned long tokens_length)
{
	unsigned long i;
	for(i = 0; i < tokens_length; i++)
	{
		if(tokens[i].token == CLAT_TOK_ATOM)
			free(tokens[i].data);
	}
	free(tokens);
}