#include "interpreter.h"
#include "private_interpreter.h"
#include "parser.h"
#include "simplic.h"
#include "simplicError.h"
#include <string.h>

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
    return makeError(error, "Variable not initialized", ERROR_ACCESS_TO_UNDECLARED_VAR);
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
    setError(error, "Variable not initialized", ERROR_ACCESS_TO_UNDECLARED_VAR);
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
    return makeError(error, "Variable not initialized", ERROR_ACCESS_TO_UNDECLARED_VAR);
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
    return makeError(error, "Tried to unset undeclared variable", ERROR_ACCESS_TO_UNDECLARED_VAR);
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

SimplicValue eval_returnInt(int n) {
    return (SimplicValue){ .type = VALUE_INT, .integer = n, .string = NULL, .receivedReturn = true };
}

SimplicValue eval_returnStr(char* s) {
    return (SimplicValue){ .type = VALUE_INT, .integer = 0, .string = s, .receivedReturn = true };
}

SimplicValue eval_makeResultInt(int n) {
    return (SimplicValue){ .type = VALUE_INT, .integer = n, .string = NULL, .receivedReturn = false };
}

SimplicValue eval_makeResultStr(char* s) {
    SimplicValue res = { .type = VALUE_STR, .integer = 0, .string = NULL, .receivedReturn = false };
    int len = strlen(s);
    res.string = malloc(sizeof(char)*(len+1));
    strcpy(res.string, s);

    return res;
}

SimplicValue eval_makeResultVoid() {
    return (SimplicValue){ .type = VALUE_VOID, .integer = 0, .string = NULL , .receivedReturn = false };
}

SimplicValue eval_makeError(SimplicError* err, const char* msg, int code) {
    return (SimplicValue){ .type = 0, .integer = 0, .string = NULL, .receivedReturn = false };
    setError(err, msg, code);
}

SimplicValue eval_makeError_keepErrInfo(SimplicError* err) {
    return (SimplicValue){ .type = 0, .integer = 0, .string = NULL, .receivedReturn = false };
    setError(err, err->errMsg, err->errCode);
}

SimplicValue eval(SyntaxNode* node, SimplicError* error) { 
    if(error->hasError) return eval_makeError_keepErrInfo(error);
    if(node->type == NODE_NUMBER) return eval_makeResultInt(node->numberValue);
    if(node->type == NODE_STRING){
        SimplicValue res = eval_makeResultStr(node->string);
        return res;
    }
    if(node->type == NODE_VAR){
        if(varIsInt(node->varName, error) && !error->hasError){
            // Variable is an int
            BankResult res = getInt(node->varName, error);
            if(res.hasError){
                return eval_makeError_keepErrInfo(error); // Requested var was not initialized
            } else {
                return eval_makeResultInt(res.integer);
            }
        } 
        else if(!error->hasError) {
            // Variable is a string
            BankResult res = getStr(node->varName, error);
            if(res.hasError){
                return eval_makeError_keepErrInfo(error); // Requested var was not initialized
            } else {
                return eval_makeResultStr(res.string);
            }
        } else {
            return eval_makeError_keepErrInfo(error);
        }
    }
    if(node->type == NODE_BIN_OP){
        SimplicValue l = eval(node->left, error);
        SimplicValue r = eval(node->right, error);
        if(error->hasError) return eval_makeError_keepErrInfo(error);

        // String concat
        if (l.type == VALUE_STR && r.type == VALUE_STR &&  (strcmp(node->operator, "+") == 0)) {
            int len = strlen(l.string) + strlen(r.string);
            char* buffer = malloc(sizeof(char)*(len+1));
            snprintf(buffer, len+1, "%s%s", l.string, r.string);
            
            SimplicValue res = eval_makeResultStr(buffer);
            free(buffer);
            free(l.string); // We are done using the string values, so we free them
            free(r.string);
            return res;
        }

        // String and number concat
        if (l.type == VALUE_STR && r.type == VALUE_INT && (strcmp(node->operator, "+") == 0)) {
            int len = strlen(l.string) + CHARS_FOR_INT_TO_STRING;
            char* buffer = malloc(sizeof(char)*(len+1));
            snprintf(buffer, len+1, "%s%d", l.string, r.integer);

            SimplicValue res = eval_makeResultStr(buffer);
            free(buffer);
            free(l.string);
            return res;
        }

        if(l.type == VALUE_INT && r.type == VALUE_STR && (strcmp(node->operator, "+") == 0)) {
            int len = strlen(r.string) + CHARS_FOR_INT_TO_STRING;
            char* buffer = malloc(sizeof(char)*(len+1));
            snprintf(buffer, len+1, "%d%s", l.integer, r.string);

            SimplicValue res = eval_makeResultStr(buffer);
            free(buffer);
            free(r.string);
            return res;
        }

        // Arithmetic operations
        if ((strcmp(node->operator, "+") == 0)) return eval_makeResultInt(l.integer + r.integer);
        if ((strcmp(node->operator, "-") == 0)) return eval_makeResultInt(l.integer - r.integer);
        if ((strcmp(node->operator, "*") == 0)) return eval_makeResultInt(l.integer * r.integer);
        if ((strcmp(node->operator, "/") == 0)) {
            if(r.integer == 0){
                return eval_makeError(error, "Division by 0, execution halted", ERROR_DIVISION_BY_ZERO);
            } else {
                return eval_makeResultInt(l.integer / r.integer);
            }
        }
        if ((strcmp(node->operator, "%") == 0)) return eval_makeResultInt(l.integer % r.integer);

        // Relational operations
        if ((strcmp(node->operator, "<") == 0)){ return eval_makeResultInt((l.integer < r.integer)? 1 : 0); }
        if ((strcmp(node->operator, "<=") == 0)){ return eval_makeResultInt((l.integer <= r.integer)? 1 : 0); }
        if ((strcmp(node->operator, ">") == 0)){ return eval_makeResultInt((l.integer > r.integer)? 1 : 0); }
        if ((strcmp(node->operator, ">=") == 0)){ return eval_makeResultInt((l.integer >= r.integer)? 1 : 0); }

        // Equality operations
        if ((strcmp(node->operator, "==") == 0)){ return eval_makeResultInt((l.integer == r.integer)? 1 : 0); }
        if ((strcmp(node->operator, "!=") == 0)){ return eval_makeResultInt((l.integer != r.integer)? 1 : 0); }

        // Logical operations
        if ((strcmp(node->operator, "&&") == 0)){ return eval_makeResultInt((l.integer && r.integer)? 1 : 0); }
        if ((strcmp(node->operator, "||") == 0)){ return eval_makeResultInt((l.integer || r.integer)? 1 : 0); }
    }
    if(node->type == NODE_ASSIGN){
        SimplicValue val = eval(node->right, error);
        if (error->hasError) return eval_makeError_keepErrInfo(error);

        if (val.type == VALUE_INT) {
            insertInt(node->varName, val.integer);
        } else if (val.type == VALUE_STR) {
            insertStr(node->varName, val.string);
            free(val.string);
        }
        return eval_makeResultVoid();
    }
    if(node->type == NODE_UNASSIGN){
        deleteVariable(node->varName, error);
        if (error->hasError) return eval_makeError_keepErrInfo(error);
        return eval_makeResultVoid();
    }
    if(node->type == NODE_PRINT || node->type == NODE_PRINTLN){
        SimplicValue val = eval(node->right, error);
        if (error->hasError) return eval_makeError_keepErrInfo(error);
        const char* delimiter = (node->type == NODE_PRINTLN)? "\n" : ""; // Add \n if PRINTLN

        if (val.type == VALUE_INT) {
            printf("%d%s", val.integer, delimiter);
        } else if (val.type == VALUE_STR) {
            printf("%s%s", val.string, delimiter);
            free(val.string);
        }
        return eval_makeResultVoid();
    }
    if(node->type == NODE_RETURN){
        SimplicValue val = eval(node->right, error);
        if (error->hasError) return eval_makeError_keepErrInfo(error);
        
        if (val.type == VALUE_INT) {
            return eval_returnInt(val.integer); // Sets a flag indicating that the last instruction was a return, used to stop eval()
        } else if (val.type == VALUE_STR) {
            return eval_returnStr(val.string);
        }
    }

    if(node->type == NODE_INCREMENT){
        SimplicValue val = eval(node->right, error);

        if(val.type == VALUE_INT) {
            if (error->hasError) return eval_makeError_keepErrInfo(error);

            val.integer++;
            insertInt(node->right->varName, val.integer);
        }
        return eval_makeResultVoid();
    }

    if(node->type == NODE_DECREMENT){
        SimplicValue val = eval(node->right, error);

        if(val.type == VALUE_INT) {
            if (error->hasError) return eval_makeError_keepErrInfo(error);

            val.integer--;
            insertInt(node->right->varName, val.integer);
        }
        return eval_makeResultVoid();
    }

    return eval_makeError(error, "Tried to evaluate unknown type node", ERROR_MISC);
}
