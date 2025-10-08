#include "unity.h"
#include "unity_internals.h"

#include "lexer.h"

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
    Token* myList;
    initTokenList(&myList);

    addTokenToTail(&myList, TOKEN_PRINT, "One");
    addTokenToTail(&myList, TOKEN_EQUALS, "Two");
    addTokenToTail(&myList, TOKEN_NUMBER, "Three");
    addTokenToTail(&myList, TOKEN_VAR, "Four");
    addTokenToTail(&myList, TOKEN_EOF, "Five");

    removeTokenFromHead(&myList);

    // Create fake temp stdout file to compare output
    char buffer[128];
    FILE *original_stdout = stdout;
    FILE *fake_stdout = tmpfile();

    stdout = fake_stdout;

    printTokens(myList);

    fflush(fake_stdout);
    fseek(fake_stdout, 0, SEEK_SET);
    fread(buffer, 1, sizeof(buffer), fake_stdout);
    buffer[strcspn(buffer, "\0")] = '\0';

    stdout = original_stdout; // restore stdout
    fclose(fake_stdout);

    TEST_ASSERT_EQUAL_STRING("Two Three Four Five\n", buffer);
    removeAllTokens(&myList);
}

void tokenizeWorks(){
    const char* program = "SET X = 34\nPRINT X\0";
    Token* myList; initTokenList(&myList);
    Token* testList; initTokenList(&testList);

    addTokenToTail(&testList, TOKEN_SET, "SET");
    addTokenToTail(&testList, TOKEN_VAR, "X");
    addTokenToTail(&testList, TOKEN_EQUALS, "=");
    addTokenToTail(&testList, TOKEN_NUMBER, "34");
    addTokenToTail(&testList, TOKEN_NEWLINE, "\\n");

    addTokenToTail(&testList, TOKEN_PRINT, "PRINT");
    addTokenToTail(&testList, TOKEN_VAR, "X");
    addTokenToTail(&testList, TOKEN_EOF, "");

    tokenizeSource(&myList, program);

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
        TEST_ASSERT_EQUAL_STRING_MESSAGE(currentTestToken->text, currentToken->text, errmsg);
        TEST_ASSERT_TRUE_MESSAGE(currentTestToken->type == currentToken->type, errmsg);

        currentToken = currentToken->next;
        currentTestToken = currentTestToken->next;
    }

    removeAllTokens(&myList);
    removeAllTokens(&testList);
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
