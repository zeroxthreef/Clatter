#ifndef TYPES_H__
#define TYPES_H__

#include <stdint.h>

/* LEXER */

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
	CLAT_TYPE_NUMBER,
	CLAT_TYPE_STRING,
	CLAT_TYPE_MAPPING,
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

} clat_ast_node_block_t;

typedef struct
{
	/* NOTE make an array of functions inside the block */
	uint32_t symbol;
} clat_ast_node_func_def_t;

typedef struct clat_ast_node_t
{
	uint8_t type;
	uint32_t num_children;
	void *data;
	struct clat_ast_node_t *children;
} clat_ast_node_t;


/* META */

/* holds current variable symbols, their atoms/identifiers (really only globals retain their text), and AST execution flags) */
typedef struct
{

} clat_state_t;

typedef struct
{
	clat_ast_node_t *root;
	/* make a list of functions */
	void *user_data;
} clat_ctx_t;

#endif