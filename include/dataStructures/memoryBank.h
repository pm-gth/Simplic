#ifndef MEMORYBANK_H
#define MEMORYBANK_H

/*
=======================================================================================
The memory bank is the structure where variables are stored and accessed during the
program's execution. Values are stored inside a hashmap where the name (aka identifier)
is used to generate the hask key.
Values from the memory bank are returned in a wrapper type so errors can be notified
=======================================================================================
*/

#include "simplic.h"
#include "simplicError.h"

typedef enum {
    VALUE_INT,
    VALUE_STR,
    VALUE_VOID
} ValueType;

void initMemoryBank(void);
void deleteMemoryBank(void); // Empties the variable bank, call at exit

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

// Assigns a value to a variable or adds it to the bank in case it didn't exist 
void insertInt(const char* key, int value); 
void insertStr(const char* key, const char* str);

// Get a variable from the bank
BankResult getInt(const char* key, SimplicError* error);
BankResult getStr(const char* key, SimplicError* error);
bool varIsInt(const char* key, SimplicError* error); // Checks if certain stored variable is an integer
BankResult deleteVariable(const char* key, SimplicError* error); // Return 0 if variable was unset successfuly, else -1

#endif