#include "lexer.h"
#include "private_parser.h"
#include "unity.h"
#include "unity_internals.h"

#include "parser.c"
#include <string.h>

Token*tokenList;

void setUp(void) {
    tokenList = initTokenQueue();
}

void tearDown(void) {
    emptyTokenQueue(&tokenList);
}

void testParseSet(void){
    const char* program = "SET X = 7 + 1\n";
    SimplicError* error = initError();
    tokenizeSource(&tokenList, program, error);

    ParseResult result = parseStatement(&tokenList, error);
    TEST_ASSERT_FALSE(result.hasError);

        // Premade correct tree
        SyntaxNode* root = initNode();
        root->type = NODE_ASSIGN;
        strcpy(root->varName, "X");

        root->subnodeB = initNode();
        root->subnodeB->type = NODE_BIN_OP;
        strcpy(root->subnodeB->operator, "+");

        root->subnodeB->subnodeB = initNode();
        root->subnodeB->subnodeB->type = NODE_NUMBER;
        root->subnodeB->subnodeB->numberValue = 1;

        root->subnodeB->subnodeA = initNode();
        root->subnodeB->subnodeA->type = NODE_NUMBER;
        root->subnodeB->subnodeA->numberValue = 7;

    TEST_ASSERT_TRUE(compareSyntaxTree(result.node, root));
    freeSyntaxTree(result.node);
    freeSyntaxTree(root);

    emptyTokenQueue(&tokenList);
    deleteError(&error);
}

void testParsePrint(void){
    const char* program = "PRINT 4\n";
    SimplicError* error = initError();
    tokenizeSource(&tokenList, program, error);

    ParseResult result = parseStatement(&tokenList, error);
    TEST_ASSERT_FALSE(result.hasError);

        // Premade correct tree
        SyntaxNode* root = initNode();
        root->type = NODE_PRINT;

        root->subnodeB = initNode();
        root->subnodeB->type = NODE_NUMBER;
        root->subnodeB->numberValue = 4;

    TEST_ASSERT_TRUE(compareSyntaxTree(result.node, root));
    freeSyntaxTree(result.node);
    freeSyntaxTree(root);

    emptyTokenQueue(&tokenList);
    deleteError(&error);
}

void testParseReturn(void){
    const char* program = "RETURN $";
    SimplicError* error = initError();
    tokenizeSource(&tokenList, program, error);

    ParseResult result = parseStatement(&tokenList, error);
    TEST_ASSERT_TRUE(result.hasError);

    freeSyntaxTree(result.node);
    emptyTokenQueue(&tokenList);
    deleteError(&error);
}

void testParseSetDeclarationOnly(void){
    const char* program = "SET Y";
    SimplicError* error = initError();
    tokenizeSource(&tokenList, program, error);

    ParseResult result = parseStatement(&tokenList, error);
    TEST_ASSERT_FALSE(result.hasError);

        // Premade correct tree
        SyntaxNode* root = initNode();
        root->type = NODE_ASSIGN;
        strcpy(root->varName, "Y");

        root->subnodeB = initNode();
        root->subnodeB->type = NODE_NUMBER;
        root->subnodeB->numberValue = 0;

    TEST_ASSERT_TRUE(compareSyntaxTree(result.node, root));
    freeSyntaxTree(result.node);
    freeSyntaxTree(root);

    emptyTokenQueue(&tokenList);
    deleteError(&error);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testParseSet);
	RUN_TEST(testParsePrint);
	RUN_TEST(testParseReturn);
    RUN_TEST(testParseSetDeclarationOnly);
    return UNITY_END();
}
