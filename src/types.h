#ifndef TYPES_H__
#define TYPES_H__

#include <stdint.h>

#define CLATTER_VER_MAJOR 0
#define CLATTER_VER_MINOR 0
#define CLATTER_VER_REVISION 1

/* UTIL TYPES */
#define FNV_OFFSET 0xcbf29ce484222325LL
#define FNV_PRIME 0x100000001b3LL

typedef struct
{
	uint8_t type;
	void *key, *value;
} clat_table_row_t;

typedef struct clat_table_t
{
	clat_table_row_t *rows;
	uint32_t row_num;
	uint8_t (*compare)(uint8_t type, void *key, void *test);
	void (*destroy)(void *row_struct);
} clat_table_t;

/* LEXER */

enum clat_parser_options
{
	CLAT_PARSE_REGULAR,
	CLAT_PARSE_DEBUG = 2
};

enum clat_tokens
{
	CLAT_TOK_PARENTH_OPEN, /*(*/
	CLAT_TOK_PARENTH_CLOSE, /*)*/
	CLAT_TOK_BRACE_OPEN, /*{*/
	CLAT_TOK_BRACE_CLOSE, /*}*/
	CLAT_TOK_AMPERSAND, /*&*/
	CLAT_TOK_COLON, /*:*/
	CLAT_TOK_STRING,
	CLAT_TOK_ATOM /* literally everything separated either by whitespace or the previous tokens */
};

typedef struct
{
	uint8_t token;
	void *data;
	#ifdef REMEMBER_LINES
	uint32_t line;
	#endif
} clat_token_t;



/* PARSER */

enum clat_nodes
{
	/* stores the functions declared */
	CLAT_NODE_BLOCK,
	CLAT_NODE_FUNCTION_CALL,
	CLAT_NODE_FUNCTION_INDETERMINATE,
	CLAT_NODE_FUNCTION_DEFINITION,
	CLAT_NODE_ATOM_LITERAL,
	CLAT_NODE_REFERENCE_ATOM_LITERAL,
	CLAT_NODE_NUMBER_LITERAL,
	CLAT_NODE_STRING_LITERAL
};

enum clat_types
{
	CLAT_TYPE_NONE,
	CLAT_TYPE_NUMBER,
	CLAT_TYPE_STRING,
	CLAT_TYPE_MAPPING,
	CLAT_TYPE_FUNCTION,
	CLAT_TYPE_ATOM,
	CLAT_TYPE_POINTER,
	CLAT_TYPE_PROGRAM,
	CLAT_TYPE_BLOCK
};

typedef struct
{
	uint8_t type;
	void *value;
} clat_val_t;

typedef struct
{
	/* ast func is 0, callback func (for C extensions) is 1 */
	uint8_t type;
	uint32_t symbol;
	void *identifier;
	/* the ast_node can be an actual node or function pointer for C */
	void *ast_node; /* pointer to the actual function node */
} clat_ast_function_t;

typedef struct
{
	/* resolve symbols at execute time */
	/* TODO add static var table */
	clat_ast_function_t *functions;
	uint32_t function_num;
} clat_ast_node_block_t;

typedef struct
{
	uint32_t symbol;
	void *identifier;
} clat_ast_node_func_call_t;

typedef struct clat_ast_node_t
{
	uint8_t type;
	uint32_t num_children;
	void *data;
	struct clat_ast_node_t *children;
} clat_ast_node_t;


/* META */

typedef struct
{
	/* dont need to keep track of weak references at all */
	uint16_t references;
	clat_val_t value;
} clat_object_t;

typedef struct
{
	clat_object_t *objects;
	uint32_t object_num;
} clat_object_list_t;

typedef struct
{
	uint32_t symbol;
	clat_object_t *object;
	void *identifier;
} clat_var_t;

typedef struct
{
	clat_ast_node_t *root;
	/* make a list of functions */
	void *user_data;
	clat_table_t *symbols;
	/* "locals" are the immediate locals.
	This is used to keep track of local
	ifetimes of variables declared inside */
	clat_table_t *locals;
	clat_object_list_t objects;
	
} clat_ctx_t;

enum clat_lang_table_types
{
	CLAT_TABLE_TYPE_CALLBACK,
	CLAT_TABLE_TYPE_VARIABLE,
	CLAT_TABLE_TYPE_STATIC_VARIABLE,
	CLAT_TABLE_TYPE_FUNCTION
};

enum clat_callback_flags
{
	CLAT_CALLBACK_REGULAR,
	CLAT_CALLBACK_WANT_LAST_RETURN = 1,
	CLAT_CALLBACK_WANT_NEXT_BLOCK = 2
};

enum clat_stdlib_flags
{
	CLAT_STDLIB_BARE,
	CLAT_STDLIB_BASE,
	CLAT_STDLIB_FULL
};

typedef struct
{
	clat_val_t(*clat_callback)(clat_ctx_t *ctx, clat_val_t *arguments, uint16_t argument_num);
	uint8_t flags;
} clat_callback_t;

#endif