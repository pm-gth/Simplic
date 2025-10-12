#include "lexer.h"
#include "private_parser.h"
#include "unity.h"
#include "unity_internals.h"

#include "parser.c"
#include <string.h>

Token*tokenList;

void setUp(void) {
    tokenList = initTokenList();
}

void tearDown(void) {
    emptyTokenList(&tokenList);
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

        root->right = initNode();
        root->right->type = NODE_BIN_OP;
        strcpy(root->right->operator, "+");

        root->right->right = initNode();
        root->right->right->type = NODE_NUMBER;
        root->right->right->numberValue = 1;

        root->right->left = initNode();
        root->right->left->type = NODE_NUMBER;
        root->right->left->numberValue = 7;

    TEST_ASSERT_TRUE(compareSyntaxTree(result.node, root));
    freeSyntaxTree(result.node);
    freeSyntaxTree(root);

    emptyTokenList(&tokenList);
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

        root->right = initNode();
        root->right->type = NODE_NUMBER;
        root->right->numberValue = 4;

    TEST_ASSERT_TRUE(compareSyntaxTree(result.node, root));
    freeSyntaxTree(result.node);
    freeSyntaxTree(root);

    emptyTokenList(&tokenList);
    deleteError(&error);
}

void testParseReturn(void){
    const char* program = "RETURN $";
    SimplicError* error = initError();
    tokenizeSource(&tokenList, program, error);

    ParseResult result = parseStatement(&tokenList, error);
    TEST_ASSERT_TRUE(result.hasError);

    freeSyntaxTree(result.node);
    emptyTokenList(&tokenList);
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

        root->right = initNode();
        root->right->type = NODE_NUMBER;
        root->right->numberValue = 0;

    TEST_ASSERT_TRUE(compareSyntaxTree(result.node, root));
    freeSyntaxTree(result.node);
    freeSyntaxTree(root);

    emptyTokenList(&tokenList);
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
