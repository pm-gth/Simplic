#include "unity.h"
#include "unity_internals.h"

#include "lexer.h"
#include "simplicError.h"
#include "parser.h"


void setUp(void) {
    ;
}

void tearDown(void) {
    ;
}

void testParseSet(void){
    const char* program = "SET X = 7 + 1\n";
    tokenizeSource(&tokenList, program);
    SimplicError* error = initError();

    ParseResult result = parseStatement(error);
    TEST_ASSERT_FALSE(result.hasError);

        // Premade correct tree
        SyntaxNode* root = malloc(sizeof(SyntaxNode));
        root->type = NODE_ASSIGN;
        strcpy(root->varName, "X");

        root->right = malloc(sizeof(SyntaxNode));
        root->right->type = NODE_BIN_OP;
        root->right->operator = '+';

        root->right->right = malloc(sizeof(SyntaxNode));
        root->right->right->type = NODE_NUMBER;
        root->right->right->numberValue = 1;

        root->right->left = malloc(sizeof(SyntaxNode));
        root->right->left->type = NODE_NUMBER;
        root->right->left->numberValue = 7;

    TEST_ASSERT_TRUE(compareSyntaxTree(result.node, root));
    freeSyntaxTree(result.node);
    freeSyntaxTree(root);

    removeAllTokens(&tokenList);
    deleteError(&error);
}

void testParsePrint(void){
    const char* program = "PRINT 4\n";
    tokenizeSource(&tokenList, program);
    SimplicError* error = initError();

    ParseResult result = parseStatement(error);
    TEST_ASSERT_FALSE(result.hasError);

        // Premade correct tree
        SyntaxNode* root = malloc(sizeof(SyntaxNode));
        root->type = NODE_PRINT;

        root->right = malloc(sizeof(SyntaxNode));
        root->right->type = NODE_NUMBER;
        root->right->numberValue = 4;

    TEST_ASSERT_TRUE(compareSyntaxTree(result.node, root));
    freeSyntaxTree(result.node);
    freeSyntaxTree(root);

    removeAllTokens(&tokenList);
    deleteError(&error);
}

void testParseReturn(void){
    const char* program = "RETURN $";
    tokenizeSource(&tokenList, program);
    SimplicError* error = initError();

    ParseResult result = parseStatement(error);
    TEST_ASSERT_TRUE(result.hasError);

    freeSyntaxTree(result.node);
    removeAllTokens(&tokenList);
    deleteError(&error);
}

void testParseSetDeclarationOnly(void){
    const char* program = "SET Y";
    tokenizeSource(&tokenList, program);
    SimplicError* error = initError();

    ParseResult result = parseStatement(error);
    TEST_ASSERT_FALSE(result.hasError);

        // Premade correct tree
        SyntaxNode* root = malloc(sizeof(SyntaxNode));
        root->type = NODE_ASSIGN;
        strcpy(root->varName, "Y");

        root->right = malloc(sizeof(SyntaxNode));
        root->right->type = NODE_NUMBER;
        root->right->numberValue = 0;

    TEST_ASSERT_TRUE(compareSyntaxTree(result.node, root));
    freeSyntaxTree(result.node);
    freeSyntaxTree(root);

    removeAllTokens(&tokenList);
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
