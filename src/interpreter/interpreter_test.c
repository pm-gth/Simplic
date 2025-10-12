#include "unity.h"
#include "unity_internals.h"

#include "interpreter.c"

Token* tokenList;
SimplicError* error;
SyntaxNode* tree;

void setUp(void) {
    tokenList = initTokenList();
    initMemoryBank();
    error = initError();
}

void tearDown(void) {
    emptyTokenList(&tokenList);
    emptyMemoryBank();
    deleteError(&error);
}

// ADD TESTS FOR PRIVATE FUNCTIONS !!! (BANK)

void testReturnCorrectInt(void) {
     const char* program =
        "SET X = 7\n"
        "RETURN X\n";

    bool end = false;
    SimplicValue val;
    while(!end){
        tokenizeSource(&tokenList, program, error);
        tree = parseTokenList(&tokenList, error);
        val = eval(tree, error);

        if(val.receivedReturn || error->hasError)
            end = true;

        freeSyntaxTree(tree);
    }

    TEST_ASSERT_FALSE(error->hasError);
    TEST_ASSERT_EQUAL_INT(7, val.integer);
}

void testComplexOperations(void) {
     const char* program =
        "SET X = 2 * 4 + 3\n"
        "INCR X\n"
        "DECR X\n"
        "SET X = X * 2\n"
        "RETURN X\n";

    bool end = false;
    SimplicValue val;
    while(!end){
        tokenizeSource(&tokenList, program, error);
        tree = parseTokenList(&tokenList, error);
        val = eval(tree, error);

        if(val.receivedReturn || error->hasError)
            end = true;

        freeSyntaxTree(tree);
    }

    TEST_ASSERT_FALSE(error->hasError);
    TEST_ASSERT_EQUAL_INT(22, val.integer);
}

void accessToUndeclaredVariable(void) {
     const char* program =
        "SET X = X\n";

    bool end = false;
    SimplicValue val;
    while(!end){
        tokenizeSource(&tokenList, program, error);
        tree = parseTokenList(&tokenList, error);
        val = eval(tree, error);

        if(val.receivedReturn || error->hasError) 
            end = true;

        freeSyntaxTree(tree);
    }

    TEST_ASSERT_TRUE(error->hasError);
}

void divisionByZero(void) {
     const char* program =
        "SET X = 9\n"
        "SET Y = 0\n"
        "SET Z = X / Y\n"
        "PRINT Z = X\n"; // <-- CHECK THIS

    bool end = false;
    SimplicValue val;
    while(!end){
        tokenizeSource(&tokenList, program, error);
        tree = parseTokenList(&tokenList, error);
        val = eval(tree, error);

        if(val.receivedReturn || error->hasError) 
            end = true;

        freeSyntaxTree(tree);
    }

    TEST_ASSERT_TRUE(error->hasError);
}

void stringConcatenation(void) {
     const char* program =
        "SET X = \"HELLO\"\n"
        "SET Y = \"WORLD\"\n"
        "RETURN X + \" \" + Y\n";

    bool end = false;
    SimplicValue val;
    while(!end){
        tokenizeSource(&tokenList, program, error);
        tree = parseTokenList(&tokenList, error);
        val = eval(tree, error);

        if(val.receivedReturn || error->hasError) 
            end = true;

        freeSyntaxTree(tree);
    }

    TEST_ASSERT_FALSE(error->hasError);
    TEST_ASSERT_EQUAL_STRING("HELLO WORLD", val.string);
    free(val.string);
}

void stringAndNumberConcatenation(void) {
     const char* program =
        "SET X = 2 * 76 % 3\n"
        "SET Y = \"YOUR LUCKY NUMBER IS: \"\n"
        "RETURN Y + X\n";

    bool end = false;
    SimplicValue val;
    while(!end){
        tokenizeSource(&tokenList, program, error);
        tree = parseTokenList(&tokenList, error);
        val = eval(tree, error);

        if(val.receivedReturn || error->hasError) 
            end = true;

        freeSyntaxTree(tree);
    }

    TEST_ASSERT_FALSE(error->hasError);
    TEST_ASSERT_EQUAL_STRING("YOUR LUCKY NUMBER IS: 2", val.string);
    free(val.string);
}

void relationalOperationsGreater(void) {
     const char* program =
        "SET X = 1 GT 0"
        "RETURN 54 + X GEQ 55";

    bool end = false;
    SimplicValue val;
    while(!end){
        tokenizeSource(&tokenList, program, error);
        tree = parseTokenList(&tokenList, error);
        val = eval(tree, error);

        if(val.receivedReturn || error->hasError) 
            end = true;

        freeSyntaxTree(tree);
    }

    TEST_ASSERT_FALSE(error->hasError);
    TEST_ASSERT_EQUAL_INT(1, val.integer);
    free(val.string);
}

void relationalOperationsLower(void) {
     const char* program =
        "SET X\n"
        "RETURN X LEQ 54 LT 3";

    bool end = false;
    SimplicValue val;
    while(!end){
        tokenizeSource(&tokenList, program, error);
        tree = parseTokenList(&tokenList, error);
        val = eval(tree, error);

        if(val.receivedReturn || error->hasError) 
            end = true;

        freeSyntaxTree(tree);
    }

    TEST_ASSERT_FALSE(error->hasError);
    TEST_ASSERT_EQUAL_INT(1, val.integer);
    free(val.string);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testReturnCorrectInt);
    RUN_TEST(testComplexOperations);
    RUN_TEST(accessToUndeclaredVariable);
    RUN_TEST(divisionByZero);
    RUN_TEST(stringConcatenation);
    RUN_TEST(stringAndNumberConcatenation);
    RUN_TEST(relationalOperationsGreater);
    RUN_TEST(relationalOperationsLower);
    return UNITY_END();
}
