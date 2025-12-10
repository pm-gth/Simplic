#include "interpreter.h"
#include "lexer.h"
#include "simplicError.h"
#include "scriptReader.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    SimplicError* error = initError();
    const char* program = readScriptFile(argv[1], error);

    if(error->hasError) {
        printError(error);
        return 1;
    }
    
    printf("Script %s contents:\n%s\n\nProgram Output:\n\n", argv[1], program);
    
    Token* tokenList = initTokenQueue();
    initMemoryBank();
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
    
    emptyTokenQueue(&tokenList);
    emptyMemoryBank();
    deleteError(&error);
    free((char*)program);
    return 0;
}