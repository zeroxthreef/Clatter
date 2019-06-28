#include "clat_stdlib.h"

#include "../types.h"
#include "../util.h"
#include "../clatter.h"

#include <string.h>

/* callbacks for language internals */

/* language features count as part of the stdlib */

static clat_val_t clat_cb_plus(clat_ctx_t *ctx, clat_val_t *arguments, uint16_t argument_num)
{
	clat_val_t ret;
	unsigned long i;
	uint8_t final_type = CLAT_TYPE_NUMBER;
	/* works with string concatenation, mapping extension (special case)
	array extension, block concatenation (difficult to add, but planned) */
	/* NOTE promote to the highest type and return that */

	/* TODO implement above */
	
	/* TODO remove this */
	ret.value = calloc(1, sizeof(double));


	for(i = 0; i < argument_num; i++)
	{
		//if(arguments[i].type)
		*(double *)ret.value += *(double *)arguments[i].value;
	}


	return ret;
}


static clat_val_t clat_cb_if(clat_ctx_t *ctx, clat_val_t *arguments, uint16_t argument_num)
{
	clat_val_t ret;
	double *ret_val = calloc(1, sizeof(double));

	if(!ret_val)
	{
		/* TODO handle serious error */
		return ret;
	}

	*ret_val = 0.0;

	ret.type = CLAT_TYPE_NUMBER;
	ret.value = ret_val;

	/*this function asks for the next block only so that will be the last argument */
	/* if statements should always return a numeric 1 if they executed or numeric 0 if they didnt.
	This way, if an else or elseif follows, they will know whether or not to execute */

	/* TODO determine how to handle strings, like if an empty string is 0 or not */
	/* also, handling every other type needs to be determined */
	if(arguments[0].type == CLAT_TYPE_NUMBER)
	{
		if(*(double *)arguments[1].value == 1.0)
			*ret_val = 1.0;
	}


	return ret;
}

static clat_val_t clat_cb_print(clat_ctx_t *ctx, clat_val_t *arguments, uint16_t argument_num)
{
	clat_val_t ret;
	unsigned long i;

	for(i = 0; i < argument_num; i++)
	{
		/* TODO dont print this way */
		if(arguments[i].type == CLAT_TYPE_NUMBER)
			printf("%f", *(double *)arguments[i].value);
		else if(arguments[i].type == CLAT_TYPE_STRING)
			printf("%s", arguments[i].value);
		else if(arguments[i].type == CLAT_TYPE_ATOM)
			printf("%s", arguments[i].value);
		/* Determine how to print other types 
		else if(arguments[i].type == CLAT_TYPE_BLOCK)
			printf("%s", arguments[i].value);
		*/
	}


	return ret;
}

/* ============================================================================= */


/* 
   bare = literally just the language essentials like control statements and loops
   base = pretty much all io, math, and necessary os functions
   full = everything else I plan to add for protocols like HTTP and stuff 
*/

/* TODO refine this and allow blocklists for certain functions or entire sections */
int clat_add_stdlib(clat_ctx_t *ctx, uint8_t stdlib_level)
{
	if(clat_add_function(ctx, strdup("if"), &clat_cb_if, CLAT_CALLBACK_WANT_NEXT_BLOCK))
	{
		/* handle error */
		return 1;
	}

	if(clat_add_function(ctx, strdup("+"), &clat_cb_plus, CLAT_CALLBACK_REGULAR))
	{
		/* handle error */
		return 1;
	}

	if(clat_add_function(ctx, strdup("print"), &clat_cb_print, CLAT_CALLBACK_REGULAR))
	{
		/* handle error */
		return 1;
	}

	return 0;
}