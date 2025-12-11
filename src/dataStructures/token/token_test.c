#include "simplicError.h"
#include "unity.h"
#include "unity_internals.h"

#include "token.c"

void setUp(void) {
    ;
}

void tearDown(void) {
    ;
}

void tokenQueueWorks(void){
    Token* myList = initTokenQueue();

    enqueueToken(&myList, TOKEN_PRINT, "One", NULL);
    enqueueToken(&myList, TOKEN_EQUALS, "Two", NULL);
    enqueueToken(&myList, TOKEN_NUMBER, "Three", NULL);
    enqueueToken(&myList, TOKEN_VAR, "Four", NULL);
    enqueueToken(&myList, TOKEN_EOF, "Five", NULL);

    deleteFirstToken(&myList);

    // Create fake temp stdout file to compare output
    char buffer[128];
    FILE *original_stdout = stdout;
    FILE *fake_stdout = tmpfile();

    stdout = fake_stdout;

    printTokenQueue(myList);

    fflush(fake_stdout);
    fseek(fake_stdout, 0, SEEK_SET);

    int n = fread(buffer, 1, sizeof(buffer) - 1, fake_stdout);
    buffer[n] = '\0';

    stdout = original_stdout; // restore stdout
    fclose(fake_stdout);

    TEST_ASSERT_EQUAL_STRING("Two Three Four Five\n", buffer);
    deleteTokenQueue(&myList);
}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(tokenQueueWorks);
    return UNITY_END();
}
