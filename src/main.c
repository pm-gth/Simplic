#include "dataStructures/ast.h"
#include "dataStructures/jumpTable.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "scriptReader.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 0;
    }

    SimplicError* error = initError();
    const char* program = readScriptFile(argv[1], error);

    if(error->hasError) {
        printError(error);
        return 1;
    }
    
    printf("Script %s contents:\n%s\n\nProgram Output:\n\n", argv[1], program);
    
    Token* tokenList = initTokenQueue();
    initAstArray();
    initMemoryBank();
    initJumpStack();
    SimplicValue val;

    tokenizeSource(&tokenList, program, error);
    parseFullCode(&tokenList, error);

    if (error->hasError) {
            printError(error);
            return 1;
        }

    size_t lines = astArraySize();

    for (size_t i = 0; i < lines; i++) {
        if(val.type == VALUE_JUMP) {
            i = syntaxNodeToLineNumber(val.jumpAddress) + 1; // +1 avoids infinite GOBACK to GOTO recursion
            val = eval(astArray[i], error);
        } else {
            val = eval(astArray[i], error);
        }

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
    
    deleteTokenQueue(&tokenList);
    deleteAstArray();
    deleteMemoryBank();
    deleteError(&error);
    free((char*)program);
    return 0;
}