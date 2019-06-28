#ifndef CLAT_STDLIB_H__
#define CLAT_STDLIB_H__

#include "../types.h"
/* 
   bare = literally just the language essentials like control statements and loops
   base = pretty much all io, math, and necessary os functions
   full = everything else I plan to add for protocols like HTTP and stuff 
*/
int clat_add_stdlib(clat_ctx_t *ctx, uint8_t stdlib_level);

#endif