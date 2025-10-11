#include "interpreter.h"
#include "simplicError.h"

int main(void) {
    const char* code =
        "SET X\n"
        "PRINT X\n"
        "INCR X\n"
        "PRINT X"
        ;

    initTokenList(&tokenList);
    initMemoryBank();
    SimplicError* error = initError();

    tokenizeSource(&tokenList, code);
    
    for (;;) {
        ParseResult result = parseStatement(error);

        if (!result.node && !result.hasError)
            break; // Reached EOF

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

    emptyTokenList(&tokenList);
    emptyMemoryBank();
    deleteError(&error);
    return 0;
}