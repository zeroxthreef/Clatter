#include "util.h"

#include "types.h"
#include "utf8.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

short clat_asprintf(char **string, const char *fmt, ...)
{
	va_list list;
	char *temp_string = NULL;
	char *oldstring = NULL;
	int size = 0;

	if(*string != NULL)
	{
		//free(*string);
    	oldstring = *string;
	}

	va_start(list, fmt);
	size = vsnprintf(temp_string, 0, fmt, list);
	va_end(list);
	va_start(list, fmt);

	if((temp_string = malloc(size + 1)) != NULL)
	{
    	if(vsnprintf(temp_string, size + 1, fmt, list) != -1)
    	{
    		*string = temp_string;
    		if(oldstring != NULL)
			{
				free(oldstring);
			}
    	return size;
    }
    else
    {
		*string = NULL;
		if(oldstring != NULL)
		{
			free(oldstring);
		}
			return -1;
		}
	}
	va_end(list);
}

short is_whitespace_utf8(void *character)
{
	char *whitespace[] = {
		"\x9",
		"\xa",
		"\xb",
		"\xc",
		"\xd",
		"\x20",
    	"\xc2\xa0",
   		"\xe1\x9a\x80",
    	"\xe2\x80\x80",
    	"\xe2\x80\x81",
    	"\xe2\x80\x82",
    	"\xe2\x80\x83",
    	"\xe2\x80\x84",
    	"\xe2\x80\x85",
    	"\xe2\x80\x86",
    	"\xe2\x80\x87",
		"\xe2\x80\x88",
    	"\xe2\x80\x89",
    	"\xe2\x80\x8a",
    	"\xe2\x80\x8b",
    	"\xe2\x80\xaf",
    	"\xe2\x81\x9f",
    	"\xe3\x80\x80"
	};
	short i;


	for(i = 0; i < sizeof(whitespace) / sizeof(char *); i++)
		if(utf8ncmp(whitespace[i], character, 1) == 0)
			return 1;


	return 0;
}

void clat_skipto_utf8(void **source, unsigned long position)
{
	unsigned long i;
	utf8_int32_t useless = 0;
	void *source_pos = *source;

	for(i = 0; i < position; i++)
		source_pos = utf8codepoint(source_pos, &useless);

	*source = source_pos;
}

void clat_print_tokens(clat_token_t *tokens, unsigned long length)
{
	unsigned long i;

	char *value, *token;

	for(i = 0; i < length; i++)
	{
		value = "";

		switch(tokens[i].token)
		{
			case CLAT_TOK_AMPERSAND:
				token = "&";
			break;
			case CLAT_TOK_COLON:
				token = ":";
			break;
			case CLAT_TOK_ATOM:
				token = "ATOM";
				value = tokens[i].data;
			break;
			case CLAT_TOK_BRACE_OPEN:
				token = "{";
			break;
			case CLAT_TOK_BRACE_CLOSE:
				token = "}";
			break;
			case CLAT_TOK_PARENTH_OPEN:
				token = "(";
			break;
			case CLAT_TOK_PARENTH_CLOSE:
				token = ")";
			break;
			case CLAT_TOK_STRING:
				token = "STR";
				value = tokens[i].data;
			break;
			default:
				token = "ERROR";
		}
		printf("token: %s%s%s\n", token, tokens[i].token == CLAT_TOK_STRING || tokens[i].token == CLAT_TOK_ATOM ? " value: " : "", value);
	}
}

void clat_print_repetitive(clat_ctx_t *ctx, char ch, int num)
{
	int i;
	for(i = 0; i < num; i++)
		printf("%c", ch);
}