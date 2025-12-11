#ifndef PRIVATE_TOKEN_H
#define PRIVATE_TOKEN_H

#include "dataStructures/token.h"

static void freeToken(Token* token); // Destructor
static void deleteFirstToken(Token** tokenList); // Frees and removes token from list

#endif