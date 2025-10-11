#include "private_interpreter.h"
#include "parser.h"
#include "simplicError.h"

MemoryCell* MemoryBank[HASH_TABLE_SIZE];

void initMemoryBank(){
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


BankResult makeResultInt(int n) {
    return (BankResult){ .integer = n, .string = NULL, .hasError = false };
}

BankResult makeResultStr(char* s) {
    return (BankResult){ .integer = -1, .string = s, .hasError = false };
}

BankResult makeError(SimplicError* err, const char* msg, int code) {
    setError(err, msg, code);
    return (BankResult){ .integer = -1, .string = NULL, .hasError = true };
}

void insertInt(const char* key, int value) {
    unsigned int index = stringHash(key);
    MemoryCell* current = MemoryBank[index];

    // Check if it already exists
    while (current != NULL) {
        if (strcmp(current->name, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }

    // Does not exist, create it
    MemoryCell* newMemCell = malloc(sizeof(MemoryCell));

    strcpy(newMemCell->name, key);
    newMemCell->value = value;
    newMemCell->strPtr = NULL;
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
    MemoryCell* newMemCell = malloc(sizeof(MemoryCell));

    strcpy(newMemCell->name, key);

    // Copy string into var
    int len = strlen(str);
    newMemCell->strPtr = malloc(sizeof(char) * (len + 1));
    strcpy(newMemCell->strPtr, str);

    newMemCell->next = MemoryBank[index];  // In case there is a collision
    newMemCell->value = -1;
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
    return makeError(error, "Variable not initialized", ERROR_ACCESS_TO_UNDECLARED_VAR);
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
    return makeError(error, "Variable not initialized", ERROR_ACCESS_TO_UNDECLARED_VAR);
}

int delete(const char* key) {
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
            return 0;
        }
        prev = current;
        current = current->next;
    }
    return -1;
}

void emptyMemoryBank() {
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

int eval(SyntaxNode* node, SimplicError* error){
    if(error->hasError) return 0;
    if(node->type == NODE_NUMBER) return node->numberValue;
    if(node->type == NODE_VAR){
        BankResult res = getInt(node->varName, error);
        if(res.hasError){
            return -1; // Requested var was not initialized
        } else {
            return res.integer;
        }
    }
    if(node->type == NODE_BIN_OP){
        int l = eval(node->left, error);
        int r = eval(node->right, error);
        switch(node->operator){
            case '+': return l + r;
            case '-': return l - r;
            case '*': return l * r;
            case '/':
            if(r == 0){
                setError(error, "Error: Division by 0, execution halted", ERROR_DIVISION_BY_ZERO);
                return -1;
            } else {
                return l / r;
            }
            case '%': return l % r;
        }
    }
    if(node->type == NODE_ASSIGN){
        int val = eval(node->right, error);
        if(!error->hasError){
            insertInt(node->varName, val);
        }
    }
    if(node->type == NODE_PRINT){
        int val = eval(node->right, error);
        if(!error->hasError){
            printf("%d\n", val);
        }
    }
    if(node->type == NODE_RETURN){
        int val = eval(node->right, error);
        exit(val);
    }

    if(node->type == NODE_INCREMENT){
        int val = eval(node->right, error);
        if(!error->hasError){
            val++;
            insertInt(node->right->varName, val);
        }
    }

    if(node->type == NODE_DECREMENT){
        int val = eval(node->right, error);
        if(!error->hasError){
            val--;
            insertInt(node->right->varName, val);
        }
    }

    return 0;
}


