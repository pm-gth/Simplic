#include "dataStructures/memoryBank.h"

MemoryCell* MemoryBank[HASH_TABLE_SIZE];

void initMemoryBank(void){
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        MemoryBank[i] = NULL;
    }
}

unsigned long stringHash(const char *str){
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash % HASH_TABLE_SIZE;
}

MemoryCell* initMemCell(void) {
    MemoryCell* res = malloc(sizeof(MemoryCell));
    memset(res->name, 0, IDENTIFIER_SIZE);
    res->next = NULL;
    res->strPtr = NULL;
    res->value = 0;

    return res;
}

BankResult makeResultInt(int n) {
    return (BankResult){ .integer = n, .string = NULL, .hasError = false };
}

BankResult makeResultStr(char* s) {
    return (BankResult){ .integer = 0, .string = s, .hasError = false };
}

BankResult makeError(SimplicError* err, SimplicErrorType code, const char* fmt, ...) {
    if (err != NULL) {
        va_list args;
        va_start(args, fmt);

        // msg len
        int len = vsnprintf(NULL, 0, fmt, args);
        va_end(args);

        if (len >= 0) {
            char* buffer = malloc(len + 1);
            if (buffer != NULL) {
                va_start(args, fmt);
                vsnprintf(buffer, len + 1, fmt, args);
                va_end(args);

                setError(err, code, "%s", buffer);
                free(buffer);
            }
        }
    }

    return (BankResult){ .integer = -1, .string = NULL, .hasError = true };
}

void insertInt(const char* key, int value) {
    unsigned int index = stringHash(key);
    MemoryCell* current = MemoryBank[index];

    // Check if it already exists
    while (current != NULL) {
        if (strcmp(current->name, key) == 0) {
            if(current->strPtr) { // Prev value stored was a string, delete it
                free(current->strPtr);
                current->strPtr = NULL;
            }
            current->value = value;
            return;
        }
        current = current->next;
    }

    // Does not exist, create it
    MemoryCell* newMemCell = initMemCell();

    strcpy(newMemCell->name, key);
    newMemCell->value = value;
    newMemCell->next = MemoryBank[index]; // In case there is a collision
    MemoryBank[index] = newMemCell;
}

void insertStr(const char* key, const char* str) {
    unsigned int index = stringHash(key);
    MemoryCell* current = MemoryBank[index];

    // Check if it already exists
    while (current != NULL) {
        if (strcmp(current->name, key) == 0) {
            if (current->strPtr != NULL){
                free(current->strPtr);
            }

            int len = strlen(str);
            current->strPtr = malloc(sizeof(char) * (len + 1));
            strcpy(current->strPtr, str);
            return;
        }
        current = current->next;
    }

    // Does not exist, create it
    MemoryCell* newMemCell = initMemCell();

    strcpy(newMemCell->name, key);

    // Copy string into var
    int len = strlen(str);
    newMemCell->strPtr = malloc(sizeof(char) * (len + 1));
    strcpy(newMemCell->strPtr, str);

    newMemCell->next = MemoryBank[index];  // In case there is a collision
    MemoryBank[index] = newMemCell;
}


BankResult getInt(const char* key, SimplicError* error) {
    unsigned int index = stringHash(key);
    MemoryCell* current = MemoryBank[index];
    while (current != NULL) {
        if (strcmp(current->name, key) == 0) {
            return makeResultInt(current->value);
        }
        current = current->next;
    }
    return makeError(error, ERROR_ACCESS_TO_UNDECLARED_VAR, "Variable %s not initialized", key);
}

bool varIsInt(const char* key, SimplicError* error) {
    unsigned int index = stringHash(key);
    MemoryCell* current = MemoryBank[index];
    while (current != NULL) {
        if (strcmp(current->name, key) == 0) {
            return (current->strPtr == NULL);
        }
        current = current->next;
    }
    setError(error, ERROR_ACCESS_TO_UNDECLARED_VAR, "Variable %s not initialized", key);
    return false;
}

BankResult getStr(const char* key, SimplicError* error) {
    unsigned int index = stringHash(key);
    MemoryCell* current = MemoryBank[index];
    while (current != NULL) {
        if (strcmp(current->name, key) == 0) {
            return makeResultStr(current->strPtr);
        }
        current = current->next;
    }
    return makeError(error, ERROR_ACCESS_TO_UNDECLARED_VAR, "Variable %s not initialized", key);
}

BankResult deleteVariable(const char* key, SimplicError* error) {
    unsigned int index = stringHash(key);
    MemoryCell* current = MemoryBank[index];
    MemoryCell* prev = NULL;
    while (current != NULL) {
        if (strcmp(current->name, key) == 0) {
            if (prev == NULL){
                MemoryBank[index] = current->next;
            } else{
                prev->next = current->next;
            }

            if(current->strPtr != NULL){
                free(current->strPtr);
            }
                
            free(current);
            return makeResultInt(0);
        }
        prev = current;
        current = current->next;
    }
    return makeError(error, ERROR_ACCESS_TO_UNDECLARED_VAR, "Tried to unset undeclared variable %s", key);
}

void deleteMemoryBank() {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        MemoryCell* current = MemoryBank[i];
        while (current != NULL) {
            MemoryCell* temp = current;
            current = current->next;

            if (temp->strPtr != NULL)
                free(temp->strPtr);

            free(temp);
        }
        MemoryBank[i] = NULL;
    }
}