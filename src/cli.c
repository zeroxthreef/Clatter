//#define REMEMBER_LINES
#include "cli.h"

#include "util.h"
#include "lexer.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* this file contains the optional interfaces to what the library compiles to */

/* NOTE MAKE CONTROL STATEMENTS LIKE IF RETURN 1 OR 0 SO IF THERES AN ELSE AND STUFF, IT CAN DETECT THAT */

/*
switch(2)
{
	case(0)
	{

	}
	case(3)
	{

	}
	default()
	{

	}
}
*/

char *test = "printsp(im_an_atom \"wo\\to\" \"hey\" +(4 *(10 10)))",\
*test1 = "=(var1 10)\n\
=(var2 \"hey\")\n\
\n\
im_function(var1 var2)\n\
\n\
im_function(arg arg2):\n\
{\n\
	printsp(\"hey, value:\" +(5 5 5 5))\n\
}\n\
",\
*test2 = "\n\
=(global 1)\n\
\n\
func(\"test\" global)\n\
\n\
func(stringo &heyo):\n\
{\n\
	printsp(stringo heyo)\n\
	=(heyo 3)\n\
}\n\
printsp(\"after\" global)\n\
";

int main(int argc, char **argv)
{
	clat_ctx_t ctx;
	memset(&ctx, 0, sizeof(clat_ctx_t));

	clat_parse_string(&ctx, test2, 0);

	clat_parse_print(&ctx, ctx.root);


	return 0;
}