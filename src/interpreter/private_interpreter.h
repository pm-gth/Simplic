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

// Wrapper functions for the bank, return values or errors
static BankResult makeResultStr(char* s);
static BankResult makeError(SimplicError* err, const char* msg, int code);

static unsigned long stringHash(const char *str); // Hash function for strings

// Assigns a value to a variable or adds it to the bank in case it didn't exist 
static void insertInt(const char* key, int value); 
static void insertStr(const char* key, const char* str);

// Get a variable from the bank
static BankResult getInt(const char* key, SimplicError* error);
static BankResult getStr(const char* key, SimplicError* error);
bool varIsInt(const char* key, SimplicError* error); // Checks if certain stored variable is an integer
static int delete(const char* key); // Return 0 if variable was unset successfuly, else -1

// Wrapper functions for eval()
static Value eval_makeResultInt(int n);
static Value eval_makeResultStr(char* s);
Value eval_makeResultVoid(); // For empty results such as the one from PRINT
static Value eval_makeError(SimplicError* err, const char* msg, int code);
Value eval_makeError_keepErrInfo(SimplicError* err);
Value eval_return(int n); // Exits from the interpreter

#endif