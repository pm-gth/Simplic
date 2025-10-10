#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "simplic.h"
#include "parser.h"

#define HASH_TABLE_SIZE 500

typedef struct MemoryCell MemoryCell;
struct MemoryCell {
    char name[IDENTIFIER_SIZE];
    int value;
    char* strPtr; // In case of string
    MemoryCell* next; // In case of name collision
};

MemoryCell* hashTable[HASH_TABLE_SIZE]; // Hashmap of variables
void initHashTable();

unsigned long stringHash(const char *str); // Hash function for strings

void insertInt(const char* key, int value); // Assigns int to variable, if it didn't exist it creates it
void insertStr(const char* key, const char* str); // Assigns string to variable, if it didn't exist it creates it
int getInt(const char* key); // Returns int value, -1 if the variable could not be found
char* getStr(const char* key); // Returns int value, NULL if the variable could not be found
int delete(const char* key); // Return 0 if key was deleted successfuly, else -1

void emptyMemoryBank();

#endif