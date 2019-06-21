//#define REMEMBER_LINES
#include "cli.h"

#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "clatter.h"

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

int main(int argc, char **argv)
{
	clat_ctx_t ctx;
	unsigned long i;
	uint8_t print_debug = 0;
	char *file = NULL;
	size_t file_size;

	memset(&ctx, 0, sizeof(clat_ctx_t));

	/* determine if it will enter REPL mode or execute from file */
	/* commands are help, print ast and tokens after parsing, and printing the version */

	if(argc > 1)
	{
		for(i = 1; i < argc; i++)
		{
			if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
			{
				printf("%s\nclatter help\n\ncommand options:\n\t(-h,--help): print this help page\n\t(-v,--version): print the version of the clatter interpreter\n\t(-d,--debug): when used in conjunction with a specified file path, it will just parse the file and output the tokens and the AST\n\n", fancy_name);
				break;
			}
			else if(strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0)
				print_debug++;
			else if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
			{
				printf("%s\nclatter version %d.%d.%d, compiled on %s\n", fancy_name, CLATTER_VER_MAJOR, CLATTER_VER_MINOR, CLATTER_VER_REVISION, __TIMESTAMP__ );
				break;
			}
			else if(strncmp(argv[i], "-", 1) != 0)
			{
				if(!file)
					file = argv[i];
			}
			else
				printf("unknown command. Please run \"-h\" or \"--help\" to print the help text\n");
		}
	}
	else
	{
		/* repl mode */
		printf("%s\nREPL mode\n\nusage:\n\tto quit, type \"exit()\" and press enter\n\tfunctions have to have the colon on the same line, ex \"beep(status):\". Otherwise, REPL mode will interpret it as a function call\n\n", fancy_name);

		return 0;
	}


	/* if we made it here, we're parsing a file */

	/* dont execute, just print what the file parses to */
	if(print_debug)
	{
		if(!(file = clat_read_file(&ctx, 0, 0, file, &file_size)))
		{
			/* handle error */
			return 1;
		}
		printf("tokens:\n\n");
		clat_parse_string(&ctx, file, CLAT_PARSE_DEBUG);

		printf("AST:\n\n");
		clat_parse_print(&ctx, ctx.root);
	}
	else
	{

	}

	


	return 0;
}