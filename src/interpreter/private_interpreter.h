#ifndef PRIVATE_INTERPRETER_H
#define PRIVATE_INTERPRETER_H

#include "interpreter.h"

typedef struct MemoryCell MemoryCell;
struct MemoryCell {
    char name[IDENTIFIER_SIZE];
    int value;
    char* strPtr; // In case of string
    MemoryCell* next; // In case of name collision
};

extern MemoryCell* MemoryBank[HASH_TABLE_SIZE]; // Hashmap of variables

// Struct used to wrap values read from the bank and check for errors
typedef struct BankResult BankResult;
struct BankResult {
    int integer;
    char* string;
    bool hasError;
};

// Wrapper functions for the bank, return values or errors
static BankResult makeResultInt(int n);
static BankResult makeResultStr(char* s);
static BankResult makeError(SimplicError* err, SimplicErrorType code, const char* fmt, ...);

static unsigned long stringHash(const char *str); // Hash function for strings

// Assigns a value to a variable or adds it to the bank in case it didn't exist 
static void insertInt(const char* key, int value); 
static void insertStr(const char* key, const char* str);

// Get a variable from the bank
static MemoryCell* initMemCell(void); // Creates a memory cell with its default values initialized
static BankResult getInt(const char* key, SimplicError* error);
static BankResult getStr(const char* key, SimplicError* error);
static bool varIsInt(const char* key, SimplicError* error); // Checks if certain stored variable is an integer
BankResult deleteVariable(const char* key, SimplicError* error); // Return 0 if variable was unset successfuly, else -1

// Wrapper functions for eval()
static SimplicValue eval_makeResultInt(int n);
static SimplicValue eval_makeResultStr(char* s);
static SimplicValue eval_makeResultVoid(); // For empty results such as the one from PRINT
static SimplicValue eval_makeError(SimplicError* err, SimplicErrorType code, const char* fmt, ...);
static SimplicValue eval_makeError_keepErrInfo(SimplicError* err);
static SimplicValue eval_returnInt(int n); // Exits from the interpreter
static SimplicValue eval_returnStr(char* s);

#endif