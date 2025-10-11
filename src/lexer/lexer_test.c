#include "simplicError.h"
#include "unity.h"
#include "unity_internals.h"

/*
    Private functions of modules need to be static so they don't collide with other modules
    However static functions can only be used by one translation unit. A translation unit is
    composed o a file and its includes, thats why we include the source file in the tests, so
    we can test its static functions
*/

#include "lexer.c"

void setUp(void) {
    ;
}

void tearDown(void) {
    ;
}

void isAlphaWorks(void) {
    char c[6] = {'a', 'A', 'z', 'Z', 'p', 'D'};

    for(int i = 0; i < 6; i++){
        TEST_ASSERT_TRUE(isAlpha(c[i]));
    }
    
    char c1[6] = {'2', '-', '#', '\n', '%', '0'};

    for(int i = 0; i < 6; i++){
        TEST_ASSERT_FALSE(isAlpha(c1[i]));
    }
}

void isNumberWorks(void) {
    char c[6] = {'0', '9', '1', '5', '7', '8'};

    for(int i = 0; i < 6; i++){
        TEST_ASSERT_TRUE(isNumber(c[i]));
    }
    
    char c1[6] = {'a', '+', '#', '\t', 'b', 'A'};

    for(int i = 0; i < 6; i++){
        TEST_ASSERT_FALSE(isNumber(c1[i]));
    }
}

void isAlphaNumerWorks(void) {
    char c[6] = {'0', 'A', 'z', 'a', 'f', '9'};

    for(int i = 0; i < 6; i++){
        TEST_ASSERT_TRUE(isAlphaNumer(c[i]));
    }
    
    char c1[6] = {'\\', '+', '#', '\t', '!', '\''};

    for(int i = 0; i < 6; i++){
        TEST_ASSERT_FALSE(isAlphaNumer(c1[i]));
    }
}

void tokenLinkedListWorks(void){
    Token* myList = initTokenList();

    addTokenToTail(&myList, TOKEN_PRINT, "One", NULL);
    addTokenToTail(&myList, TOKEN_EQUALS, "Two", NULL);
    addTokenToTail(&myList, TOKEN_NUMBER, "Three", NULL);
    addTokenToTail(&myList, TOKEN_VAR, "Four", NULL);
    addTokenToTail(&myList, TOKEN_EOF, "Five", NULL);

    removeTokenFromHead(&myList);

    // Create fake temp stdout file to compare output
    char buffer[128];
    FILE *original_stdout = stdout;
    FILE *fake_stdout = tmpfile();

    stdout = fake_stdout;

    printTokens(myList);

    fflush(fake_stdout);
    fseek(fake_stdout, 0, SEEK_SET);
    
    int n = fread(buffer, 1, sizeof(buffer) - 1, fake_stdout);
    buffer[n] = '\0';

    stdout = original_stdout; // restore stdout
    fclose(fake_stdout);

    TEST_ASSERT_EQUAL_STRING("Two Three Four Five\n", buffer);
    emptyTokenList(&myList);
}

void tokenizeWorks(){
    const char* program = "SET X = 34\n"
                          "PRINT X\0";
                          
    SimplicError* error = initError();
    Token* myList = initTokenList();
    Token* testList = initTokenList();

    addTokenToTail(&testList, TOKEN_SET, "SET", NULL);
    addTokenToTail(&testList, TOKEN_VAR, "X", NULL);
    addTokenToTail(&testList, TOKEN_EQUALS, "=", NULL);
    addTokenToTail(&testList, TOKEN_NUMBER, "34", NULL);
    addTokenToTail(&testList, TOKEN_NEWLINE, "\\n", NULL);

    addTokenToTail(&testList, TOKEN_PRINT, "PRINT", NULL);
    addTokenToTail(&testList, TOKEN_VAR, "X", NULL);
    addTokenToTail(&testList, TOKEN_EOF, "", NULL);

    tokenizeSource(&myList, program, error);

    Token* currentToken = myList;
    Token* currentTestToken = testList;

    // Test if lists are the same size
    unsigned short int size = 0;
    unsigned short int testSize = 0;

    while(currentToken != NULL){
        size++;
        currentToken = currentToken->next;
    }

    while(currentTestToken != NULL){
        testSize++;
        currentTestToken = currentTestToken->next;
    }

    TEST_ASSERT_EQUAL_MESSAGE(testSize, size, "Error: token list size does not match its supposed value");

    // Test if they have the same contents
    currentToken = myList;
    currentTestToken = testList;


    const char* errmsg = "Error: token list contents do not match its supposed values";
    
    while(currentToken != NULL){
        TEST_ASSERT_EQUAL_STRING_MESSAGE(currentTestToken->name, currentToken->name, errmsg);
        TEST_ASSERT_TRUE_MESSAGE(currentTestToken->type == currentToken->type, errmsg);

        currentToken = currentToken->next;
        currentTestToken = currentTestToken->next;
    }

    emptyTokenList(&myList);
    emptyTokenList(&testList);
    deleteError(&error);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(isAlphaWorks);
    RUN_TEST(isNumberWorks);
    RUN_TEST(isAlphaNumerWorks);
    RUN_TEST(tokenLinkedListWorks);
    RUN_TEST(tokenizeWorks);
    return UNITY_END();
}
