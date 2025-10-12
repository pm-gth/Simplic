#include "interpreter.h"
#include "lexer.h"
#include "simplicError.h"

int main(void) {
    const char* programA =
        "SET X = 8 * 2 + 3\n"
        "INCR X\n"
        "SET Y = \"El resultado es: \"\n"
        "PRINT Y + X\n"
        "RETURN 0\n"
        ;

    const char* program =
        "SET X = 0\n"
        "SET Y = 0"
        "WHILE X LT 5\n"
        "DO\n"
            "INCR X\n"
            "WHILE Y LT 3\n"
            "DO\n"
                "INCR Y\n"
                "PRINTLN \"Y: \" + Y\n"
            "DONE\n"
            "PRINTLN X\n"
            "SET Y = 0\n"
        "DONE\n"
        "RETURN RES\n"
        ;

    Token* tokenList = initTokenList();
    initMemoryBank();
    SimplicError* error = initError();
    SimplicValue val;

    tokenizeSource(&tokenList, program, error);

    for (;;) {
        SyntaxNode* result = parseTokenList(&tokenList, error);

        if (!result && !error->hasError) {
            break; // Reached EOF with no return
        }
            
        if (error->hasError) {
            printError(error);
            break;
        }

        val = eval(result, error);
        freeSyntaxTree(result);
        

        if (error->hasError) {
            printError(error);
            break;
        }

        if (val.receivedReturn) {
            printf("Program ended with return code: %d\n", val.integer);
            break;
        }
    }

    if(val.type == VALUE_STR){
        free(val.string);
    }
    
    emptyTokenList(&tokenList);
    emptyMemoryBank();
    deleteError(&error);
    return 0;
}