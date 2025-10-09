#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>

int eval(SyntaxNode* node, SimplicError* error){
    if(error->hasError) return 0;
    if(node->type == NODE_NUMBER) return node->numberValue;
    if(node->type == NODE_VAR) return 0; // Cargar var de banco!!
    if(node->type == NODE_BIN_OP){
        int l = eval(node->left, error);
        int r = eval(node->right, error);
        switch(node->operator){
            case '+': return l + r;
            case '-': return l - r;
            case '*': return l * r;
            case '/':
            if(r == 0){
                setError(error, "Error: Division by 0, execution halted", ERROR_DIVISION_BY_ZERO);
                return -1;
            } else {
                return l / r;
            }
        }
    }
    if(node->type == NODE_ASSIGN){
        // Create named var and store value
        // Alphabetic order in var storage and search algorithm
    }
    if(node->type == NODE_PRINT){
        int val = eval(node->right, error);
        if(!error->hasError){
            printf("%d\n", val);
        }
    }
    if(node->type == NODE_RETURN){
        int val = eval(node->right, error);
        exit(val);
    }
    return 0;
}

int main(void) {
        const char* code =
        "PRINT 4 / 4\n"
        "PRINT 7\n"
        "RETURN 5 * 5";

    initTokenList(&tokenList);
    tokenizeSource(&tokenList, code);

    SimplicError* error = initError();

    for (;;) {
        ParseResult result = parseStatement(error);

        if (!result.node && !result.hasError)
            break;

        if (result.hasError) {
            printError(error);
            break;
        }

        eval(result.node, error);

        if (error->hasError) {
            printError(error);
            break;
        }

        freeSyntaxTree(result.node);
    }

    removeAllTokens(&tokenList);
    return 0;
}


/*
Programa objetivo:

SET X = 5
FOR I = 1 TO 200 BY 1
DO 
    PRINT I
    SET X = X * I 
DONE
RETURN X
*/

/*
Programa objetivo:

SET X = 67
SET Y = 0

IF X % 2 EQUALS 0
THEN 
    SET Y = 1
ELSE
    SET Y = 0
ENDIF

RETURN Y
*/

/*
Programa objetivo:

SET X = 100

WHILE X LOWEREQ 0
DO
    SET X = X -1
DONE

RETURN X
*/

/*
Programa objetivo:

SET X = 100
SET Y = 50

IF X % 2 EQUALS 0 AND Y % 2 EQUALS 0
THEN
    PRINT 1
ELSE
    PRINT 0
ENDIF

RETURN 0
*/

// IF THEN ENDIF ELSE ELSEIF
// FOR I = 0 TO 100 BY X
// WHILE DO DONE
// EQUALS, LOWER, GREATER, LOWEREQ, GREATEREQ
// AND, OR
// +, -, *. /, %