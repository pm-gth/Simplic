#include "interpreter.h"
#include "simplicError.h"

int main(void) {
    const char* code =
        "SET X = 8 * 2 + 3\n"
        "INCR X\n"
        "SET Y = \"El resultado es: \"\n"
        "PRINT Y + X\n"
        "RETURN 0\n"
        ;

    initTokenList(&tokenList);
    initMemoryBank();
    SimplicError* error = initError();

    tokenizeSource(&tokenList, code, error);

    for (;;) {
        ParseResult result = parseStatement(error);

        if (!result.node && !result.hasError)
            break; // Reached EOF with no return

        if (result.hasError) {
            printError(error);
            break;
        }

        Value val = eval(result.node, error);
        freeSyntaxTree(result.node);

        if (error->hasError) {
            printError(error);
            break;
        }

        if (val.receivedReturn) {
            printf("Program ended with return code: %d\n", val.integer);
            break;
        }
    }

    emptyTokenList(&tokenList);
    emptyMemoryBank();
    deleteError(&error);
    return 0;
}