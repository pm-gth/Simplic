#include "dataStructures/ast.h"
#include "private_ast.h"
#include "simplic.h"

SyntaxNode* initNode() {
    SyntaxNode* res = malloc(sizeof(SyntaxNode));
    res->numberValue = 0;
    strcpy(res->operator, "&");
    res->string = NULL;
    res->subnodeA = NULL;
    res->subnodeB = NULL;
    res->subnodeC = NULL;
    res->blockStatements = NULL;
    res->type = 0;

    return res;
}

void freeSyntaxTree(SyntaxNode* tree) {
    int i;

    if (tree == NULL)
        return;

    switch (tree->type) {
        case NODE_NUMBER:
        case NODE_VAR:
        case NODE_UNASSIGN:
            // No child nodes to free
            break;
        case NODE_STRING:
            free(tree->string);
            break;

        case NODE_BIN_OP:
        case NODE_WHILE:
            freeSyntaxTree(tree->subnodeA);
            freeSyntaxTree(tree->subnodeB);
            break;

        case NODE_BLOCK: // Array of ASTs
            i = 0;
            while (tree->blockStatements[i] != NULL) { // Free each AST
                freeSyntaxTree(tree->blockStatements[i++]);
            }
            free(tree->blockStatements); // Free list of ASts
            break;

        case NODE_IF:
            freeSyntaxTree(tree->subnodeA);
            freeSyntaxTree(tree->subnodeB);
            freeSyntaxTree(tree->subnodeC);
            break;

        case NODE_ASSIGN:
        case NODE_PRINT:
        case NODE_PRINTLN:
        case NODE_RETURN:
        case NODE_INCREMENT:
        case NODE_DECREMENT:
            // These have only one child on the right
            freeSyntaxTree(tree->subnodeB);
            break;

        default:
            // Undefined, try to free both branches
            freeSyntaxTree(tree->subnodeA);
            freeSyntaxTree(tree->subnodeB);
            break;
    }

    free(tree);
}

bool compareSyntaxTree(SyntaxNode* a, SyntaxNode* b) {
    int i;
    if (a == NULL && b == NULL) return true;
    if (a == NULL || b == NULL) return false;

    if(a->type != b->type)
        return false;

    switch (a->type) {
        case NODE_NUMBER:
            return a->numberValue == b->numberValue;

        case NODE_VAR:
        case NODE_UNASSIGN:
            // Vars are only compared by name, their value is in the bank
            return strcmp(a->varName, b->varName) == 0;

        case NODE_STRING:
            // Compare string contents
            return strcmp(a->string, b->string) == 0;

        case NODE_BIN_OP:
        case NODE_WHILE:
            return compareSyntaxTree(a->subnodeA, b->subnodeA) && compareSyntaxTree(a->subnodeB, b->subnodeB);

        case NODE_BLOCK:
            i = 0;
            while (a->blockStatements[i] != NULL && b->blockStatements[i] != NULL) {
                if (!compareSyntaxTree(a->blockStatements[i], b->blockStatements[i]))
                    return false;
                i++;
                }

            // One is larger than the other
            if (a->blockStatements[i] != NULL || b->blockStatements[i] != NULL)
                return false;
            return true;

        case NODE_IF:
            return compareSyntaxTree(a->subnodeA, b->subnodeA) && compareSyntaxTree(a->subnodeB, b->subnodeB) && compareSyntaxTree(a->subnodeC, b->subnodeC);

        case NODE_ASSIGN:
        case NODE_PRINT:
        case NODE_PRINTLN:
        case NODE_RETURN:
        case NODE_INCREMENT:
        case NODE_DECREMENT:
            return compareSyntaxTree(a->subnodeB, b->subnodeB);

        default:
            // Undefined, try to compare both branches
            return compareSyntaxTree(a->subnodeA, b->subnodeA) && compareSyntaxTree(a->subnodeB, b->subnodeB);
    }

    return false;
}

SyntaxNode** astArray;
size_t astArrayCapacity;
size_t astArrayIndex;

void initAstArray() {
    astArray = malloc(sizeof(SyntaxNode*) * STARTING_LINE_CAPACITY);

    astArrayCapacity = STARTING_LINE_CAPACITY;
    astArrayIndex = 0;

    for(int i = 0; i < STARTING_LINE_CAPACITY; i++) {
        astArray[i] = NULL;
    }
}

void astArrayEnsureCapacity() {
    if(astArrayIndex >= astArrayCapacity) {
        astArray = realloc(astArray, astArrayCapacity + 10);
        astArrayCapacity += 10;

        for(size_t i = astArrayIndex; i < astArrayCapacity; i++) {
            astArray[i] = NULL;
        }
    }
}

void addLineAstArray(SyntaxNode* tree) {
    astArrayEnsureCapacity();
    astArray[astArrayIndex++] = tree;
}

void deleteAstArray(void) {
    size_t i = 0;
    SyntaxNode* current = astArray[i];

    while(current != NULL && i < astArrayCapacity) {
        freeSyntaxTree(current);
        current = astArray[++i];
    }

    free(astArray);
}

size_t astArraySize(void) {
    return astArrayIndex; // Return up to the last used position
}