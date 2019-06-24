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

int clat_determine_if_number(clat_ctx_t *ctx, void *value)
{
	unsigned int i;
	long code_p = 0;

	/* TODO TODO TODO ALLOW HEX CHARS TO NOT CATCH IT OFF GUARD */

	for(i = 0; i < utf8len(value); i++)
	{
		/* if it starts with 0, and b, x, or any of the other chars follow and its a number, OR its _entirely_ numbers,
		its a number. Anything else is a pure atom. There can be decimals, so we ignore those. */
		value = utf8codepoint(value, &code_p);

		if(code_p < '0' || code_p > '9')
			return 0;
	}

	return 1;
}

uint8_t *clat_read_file(clat_ctx_t *ctx, size_t begin, size_t end, const char *path, size_t *size)
{
	uint8_t *data = NULL;
	FILE *file = NULL;

	if(!(file = fopen(path, "rb")))
	{
		/* print an error */
		return data;
	}
	
	fseek(file, 0, SEEK_END);
	*size = ftell(file);
	fseek(file, 0, SEEK_SET);

	if(!(data = (uint8_t *)calloc(1, *size + 1))) /* doesnt matter if we add _just one byte_ at the end as long as we return the real amount. Good for text file loading */
	{
		/* print an error */

		if(fclose(file))
		{
			/* print an error */
			return NULL;
		}
		return NULL;
	}

	if(fread(data, sizeof(uint8_t), *size, file) != *size)
	{
		/* print an error */

		if(fclose(file))
		{
			/* print an error */
			return NULL;
		}
		
		return NULL;
	}

	if(fclose(file))
	{
		/* print an error */
		return data; /* probably ok to return data here. Just a weird filesystem error */
	}

	return data;
}

ssize_t clat_write_file(clat_ctx_t *ctx, uint8_t data, const char *path, size_t size)
{
	FILE *file = NULL;
	size_t amount;

	if(!(file = fopen(path, "wb")))
	{
		/* print an error */
		return -1;
	}

	amount = fwrite((void *)data, sizeof(uint8_t), size, file);


	if(fclose(file))
	{
		/* print an error */
		return -1;
	}

	return amount;
}

short clat_read_bitflag(uint64_t value, short bit)
{
	/* shift then use a bitmask */
	value >>= bit;
	value &= 1;
	return value;
}

int clat_realloc_array(void **ptr, size_t currentNum, size_t wantNum, size_t sizeOfIndividualObject) /* NOTE can just pass it a NULL pointer and it allocates it and stuff */
{
	void *tempPtr = NULL;

	/* printf("pointer0 %p pointer1 %p\n", ptr, *ptr); */
	if(*ptr == NULL)
	{
		tempPtr = calloc(wantNum, sizeOfIndividualObject);

		if(tempPtr == NULL)
			return 1;
		else
			*ptr = tempPtr;
	}
	else
	{
		tempPtr = realloc(*ptr, sizeOfIndividualObject * wantNum);
		if(tempPtr == NULL)
			return 1;
		else
			*ptr = tempPtr;
	}
	return 0;
}

int clat_remove_array_entry(void **ptr, size_t currentNum, size_t positionToRemove, size_t sizeOfIndividualObject)
{
	void *destPtr = *ptr + (positionToRemove * sizeOfIndividualObject);
	void *selectptr = *ptr + ((positionToRemove + 1) * sizeOfIndividualObject);


	/* TODO check if it is the last item because there is nothing to copy after that */

	if(positionToRemove == currentNum)
	{
		if(!clat_realloc_array(ptr, currentNum - 1, currentNum - 1, sizeOfIndividualObject))
			return 0;
		else
			return 1;
	}
	else
	{
		memmove(destPtr, selectptr, sizeOfIndividualObject * (currentNum - positionToRemove));
		if(!clat_realloc_array(ptr, currentNum - 1, currentNum - 1, sizeOfIndividualObject))
		{
			return 0;
		}
		else
			return 1;
	}

	return 0;
}

int clat_add_array_entry(void **ptr, size_t currentNum, void *objectPtr, size_t sizeOfIndividualObject)
{
	if(!clat_realloc_array(ptr, currentNum, currentNum + 1, sizeOfIndividualObject))
	{
		/* ptr[currentNum + 1] = *objectPtr; */
		void *tempPtr = *ptr + (currentNum) * sizeOfIndividualObject;
		/* tempPtr = *objectPtr; */
		memcpy(tempPtr, objectPtr, sizeOfIndividualObject);
		/* printf("[%c] [%c]\n", *((char *)tempPtr), *((char *)tempPtr)); */
	}
	else
	{
		return 1;
	}

	return 0;
}

int clat_init_table(clat_table_t **table, void (*compare)(uint8_t type, void *key, void *test), void (*destroy)(struct clat_table_row_t *row))
{
	if(!(*table = calloc(1, sizeof(clat_table_t))))
	{
		/* maybe handle error */
		return 1;
	}

	

	return 0;
}

int clat_destroy_table(clat_table_t *table)
{

	return 0;
}

int clat_add_table_row(clat_table_t *table, uint8_t type, void *key, void *value)
{

	return 0;
}

int clat_remove_table_row(clat_table_t *table)
{

	return 0;
}

void *clat_value_at(clat_table_t *table)
{

}