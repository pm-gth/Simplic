#include "simplicError.h"
#include "unity.h"
#include "unity_internals.h"

#include "interpreter.c"
#include "parser.h"

Token* tokenList;
SimplicError* error;
SyntaxNode* tree;

void setUp(void) {
    tokenList = initTokenQueue();
    initMemoryBank();
    error = initError();
}

void tearDown(void) {
    deleteTokenQueue(&tokenList);
    deleteMemoryBank();
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
        tree = parseLineOfCode(&tokenList, error);
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
        tree = parseLineOfCode(&tokenList, error);
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
        tree = parseLineOfCode(&tokenList, error);
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
        tree = parseLineOfCode(&tokenList, error);
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
        tree = parseLineOfCode(&tokenList, error);
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
        tree = parseLineOfCode(&tokenList, error);
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
        tree = parseLineOfCode(&tokenList, error);
        val = eval(tree, error);

        if(val.receivedReturn || error->hasError) 
            end = true;

        freeSyntaxTree(tree);
    }

    TEST_ASSERT_FALSE(error->hasError);
    TEST_ASSERT_EQUAL_INT(1, val.integer);
}

void relationalOperationsLower(void) {
     const char* program =
        "SET X\n"
        "RETURN X LEQ 54 LT 3";

    bool end = false;
    SimplicValue val;
    while(!end){
        tokenizeSource(&tokenList, program, error);
        tree = parseLineOfCode(&tokenList, error);
        val = eval(tree, error);

        if(val.receivedReturn || error->hasError) 
            end = true;

        freeSyntaxTree(tree);
    }

    TEST_ASSERT_FALSE(error->hasError);
    TEST_ASSERT_EQUAL_INT(1, val.integer);
}

void equalityOperationsEqual(void) {
     const char* program =
        "SET X\n"
        "RETURN X EQ 54 LT 3";

    bool end = false;
    SimplicValue val;
    while(!end){
        tokenizeSource(&tokenList, program, error);
        tree = parseLineOfCode(&tokenList, error);
        val = eval(tree, error);

        if(val.receivedReturn || error->hasError) 
            end = true;

        freeSyntaxTree(tree);
    }

    TEST_ASSERT_FALSE(error->hasError);
    TEST_ASSERT_EQUAL_INT(1, val.integer);
}

void equalityOperationsNotEqual(void) {
     const char* program =
        "SET X = 99 NEQ 0\n"
        "RETURN X GT 0";

    bool end = false;
    SimplicValue val;
    while(!end){
        tokenizeSource(&tokenList, program, error);
        tree = parseLineOfCode(&tokenList, error);
        val = eval(tree, error);

        if(val.receivedReturn || error->hasError) 
            end = true;

        freeSyntaxTree(tree);
    }

    TEST_ASSERT_FALSE(error->hasError);
    TEST_ASSERT_EQUAL_INT(1, val.integer);
}

void logicalOperatorsAndOr(void) {
     const char* program =
        "SET X = 5 EQ 5 AND 7 LT 3\n"
        "SET Y = 77 LEQ 77 OR 6 GT 99\n"
        "SET RES = X OR Y\n"
        "RETURN RES\n";

    bool end = false;
    SimplicValue val;
    while(!end){
        tokenizeSource(&tokenList, program, error);
        tree = parseLineOfCode(&tokenList, error);
        val = eval(tree, error);

        if(val.receivedReturn || error->hasError) 
            end = true;

        freeSyntaxTree(tree);
    }

    TEST_ASSERT_FALSE(error->hasError);
    TEST_ASSERT_EQUAL_INT(1, val.integer);
}

void unsetVariable(void) {
     const char* program =
        "SET X = 99\n"
        "UNSET X\n"
        "RETURN X\n";

    bool end = false;
    SimplicValue val;
    while(!end){
        tokenizeSource(&tokenList, program, error);
        tree = parseLineOfCode(&tokenList, error);
        val = eval(tree, error);

        if(val.receivedReturn || error->hasError) 
            end = true;

        freeSyntaxTree(tree);
    }

    TEST_ASSERT_TRUE(error->hasError);
}

void whileLoop(void) {
     const char* program =
        "SET X = 0\n"
        "WHILE X LT 5 DO\n"
            "INCR X\n"
        "DONE\n"
        "RETURN X\n";

    bool end = false;
    SimplicValue val;
    while(!end){
        tokenizeSource(&tokenList, program, error);
        tree = parseLineOfCode(&tokenList, error);
        val = eval(tree, error);

        if(val.receivedReturn || error->hasError) 
            end = true;

        freeSyntaxTree(tree);
    }

    TEST_ASSERT_FALSE(error->hasError);
    TEST_ASSERT_EQUAL_INT(5, val.integer);
}

void nestedWhileLoop(void) {
     const char* program =
        "SET X = 0\n"
        "SET Y = 0\n"
        "SET ITERS = 0\n"
        "WHILE X LT 5 DO\n"
            "WHILE Y LT 5 DO\n"
                "INCR Y\n"
                "INCR ITERS\n"
            "DONE\n"
            "SET Y = 0\n"
            "INCR X\n"
            "INCR ITERS\n"
        "DONE\n"
        "RETURN ITERS\n";

    bool end = false;
    SimplicValue val;
    while(!end){
        tokenizeSource(&tokenList, program, error);
        tree = parseLineOfCode(&tokenList, error);
        val = eval(tree, error);

        if(val.receivedReturn || error->hasError) 
            end = true;

        freeSyntaxTree(tree);
    }

    TEST_ASSERT_FALSE(error->hasError);
    TEST_ASSERT_EQUAL_INT(30, val.integer);
}

void ifStatement(void) {
     const char* program =
        "SET X = 3\n"
        "IF X % 2 NEQ 0 THEN\n"
            "RETURN 1\n"
        "ELSE\n"
            "RETURN 0\n"
        "FI";

    bool end = false;
    SimplicValue val;
    while(!end){
        tokenizeSource(&tokenList, program, error);
        tree = parseLineOfCode(&tokenList, error);
        val = eval(tree, error);

        if(val.receivedReturn || error->hasError) 
            end = true;

        freeSyntaxTree(tree);
    }

    TEST_ASSERT_FALSE(error->hasError);
    TEST_ASSERT_EQUAL_INT(1, val.integer);
}

void elseStatement(void) {
     const char* program =
        "SET X = 4\n"
        "IF X % 2 NEQ 0 THEN\n"
            "RETURN 1\n"
        "ELSE\n"
            "RETURN 0\n"
        "FI";

    bool end = false;
    SimplicValue val;
    while(!end){
        tokenizeSource(&tokenList, program, error);
        tree = parseLineOfCode(&tokenList, error);
        val = eval(tree, error);

        if(val.receivedReturn || error->hasError) 
            end = true;

        freeSyntaxTree(tree);
    }

    TEST_ASSERT_FALSE(error->hasError);
    TEST_ASSERT_EQUAL_INT(0, val.integer);
}

// Used to check if early returns ignore the rest of the code
void returnFromNested(void) {
     const char* program =
        "SET X = 0\n"
        "WHILE X LT 5 DO\n"
            "INCR X\n"
            "IF X EQ 5 THEN\n"
                "RETURN 0\n"
            "FI\n"
        "DONE\n"
        "RETURN 1\n";

    bool end = false;
    SimplicValue val;
    while(!end){
        tokenizeSource(&tokenList, program, error);
        tree = parseLineOfCode(&tokenList, error);
        val = eval(tree, error);

        if(val.receivedReturn || error->hasError) 
            end = true;

        freeSyntaxTree(tree);
    }

    TEST_ASSERT_FALSE(error->hasError);
    TEST_ASSERT_EQUAL_INT(0, val.integer);
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
    RUN_TEST(equalityOperationsEqual);
    RUN_TEST(equalityOperationsNotEqual);
    RUN_TEST(logicalOperatorsAndOr);
    RUN_TEST(unsetVariable);
    RUN_TEST(whileLoop);
    RUN_TEST(nestedWhileLoop);
    RUN_TEST(ifStatement);
    RUN_TEST(elseStatement);
    RUN_TEST(returnFromNested);
    return UNITY_END();
}
