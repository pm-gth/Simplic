#include "interpreter.h"

int main(void) {
    const char* code =
        "SET X\n"
        "PRINT X\n"
        "INCR X\n"
        "PRINT X"
        ;

    initHashTable();
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
    emptyMemoryBank();
    return 0;
}