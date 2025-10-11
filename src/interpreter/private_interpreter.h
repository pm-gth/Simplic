#ifndef PRIVATE_INTERPRETER_H
#define PRIVATE_INTERPRETER_H

#include "interpreter.h"

// Struct used to wrap values read from the bank and check for errors
typedef struct BankResult BankResult;
struct BankResult {
    int integer;
    char* string;
    bool hasError;
};

static BankResult makeResultStr(char* s);
static BankResult makeError(SimplicError* err, const char* msg, int code);

static unsigned long stringHash(const char *str); // Hash function for strings

static void insertInt(const char* key, int value); // Assigns int to variable, if it didn't exist it creates it
static void insertStr(const char* key, const char* str); // Assigns string to variable, if it didn't exist it creates it
static BankResult getInt(const char* key, SimplicError* error);
static BankResult getStr(const char* key, SimplicError* error);
static int delete(const char* key); // Return 0 if key was deleted successfuly, else -1

#endif