#include "lexer.h"
#include "simplicError.h"
#include "unity.h"
#include "unity_internals.h"

#include "parser.h"

SimplicError* error;

void setUp(void) {
    const char* program = 
        "SET X = 7 + 1\n"
        "PRINT 4\n"
        "RETURN 5 / * 4\n"
        "PRINT 0\n"
        ;

    tokenizeSource(&tokenList, program);
    error = initError();
}

void tearDown(void) {
    removeAllTokens(&tokenList);
    deleteError(&error);
}

void testParseSet(void){
    // Parse SET X = 7+1\n
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
}

void testParsePrint(void){
    // Parse PRINT 4\n
    ParseResult result = parseStatement(error);
    freeSyntaxTree(result.node);
	result = parseStatement(error);

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
}

void testParseReturn(void){
    // RETURN 5 / * 4\n
    ParseResult result = parseStatement(error);
    freeSyntaxTree(result.node);
	result = parseStatement(error);
    freeSyntaxTree(result.node);
	result = parseStatement(error);

    TEST_ASSERT_TRUE(result.hasError);

    freeSyntaxTree(result.node);
}

// Add some more tests with complex args like 3 + 5 * 2

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testParseSet);
	RUN_TEST(testParsePrint);
	RUN_TEST(testParseReturn);
    return UNITY_END();
}
