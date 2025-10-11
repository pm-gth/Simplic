#include "interpreter.h"
#include "private_interpreter.h"
#include "parser.h"
#include "simplicError.h"
#include <stdbool.h>

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

bool varIsInt(const char* key, SimplicError* error) {
    unsigned int index = stringHash(key);
    MemoryCell* current = MemoryBank[index];
    while (current != NULL) {
        if (strcmp(current->name, key) == 0) {
            return (current->strPtr == NULL);
        }
        current = current->next;
    }
    setError(error, "VAriable not initialized", ERROR_ACCESS_TO_UNDECLARED_VAR);
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

Value eval_return(int n) {
    return (Value){ .type = VALUE_INT, .integer = n, .string = NULL, .receivedReturn = true };
}

Value eval_makeResultInt(int n) {
    return (Value){ .type = VALUE_INT, .integer = n, .string = NULL, .receivedReturn = false };
}

Value eval_makeResultStr(char* s) {
    Value res = { .type = VALUE_STR, .integer = 0, .string = NULL, .receivedReturn = false };
    int len = strlen(s);
    res.string = malloc(sizeof(char)*(len+1));
    strcpy(res.string, s);

    return res;
}

Value eval_makeResultVoid() {
    return (Value){ .type = VALUE_VOID, .integer = 0, .string = NULL , .receivedReturn = false };
}

Value eval_makeError(SimplicError* err, const char* msg, int code) {
    return (Value){ .type = 0, .integer = 0, .string = NULL, .receivedReturn = false };
    setError(err, msg, code);
}

Value eval_makeError_keepErrInfo(SimplicError* err) {
    return (Value){ .type = 0, .integer = 0, .string = NULL, .receivedReturn = false };
    setError(err, err->errMsg, err->errCode);
}

Value eval(SyntaxNode* node, SimplicError* error) { 
    if(error->hasError) return eval_makeError_keepErrInfo(error);
    if(node->type == NODE_NUMBER) return eval_makeResultInt(node->numberValue);
    if(node->type == NODE_STRING){
        Value res = eval_makeResultStr(node->string);
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
        Value l = eval(node->left, error);
        Value r = eval(node->right, error);
        if(error->hasError) return eval_makeError_keepErrInfo(error);

        // String concat
        if (l.type == VALUE_STR && r.type == VALUE_STR && node->operator == '+') {
            int len = strlen(l.string) + strlen(r.string);
            char* buffer = malloc(sizeof(char)*(len+1));
            snprintf(buffer, len+1, "%s%s", l.string, r.string);
            
            Value res = eval_makeResultStr(buffer);
            free(buffer);
            return res;
        }

        // String and number concat
        if (l.type == VALUE_STR && r.type == VALUE_INT && node->operator == '+') {
            char buffer[20]; // suficiente para un int normal
            snprintf(buffer, sizeof(buffer), "%s%d", l.string, r.integer);

            Value res = eval_makeResultStr(buffer);
            return res;
        }

        if(l.type == VALUE_INT && r.type == VALUE_STR && node->operator == '+') {
            char buffer[20]; // suficiente para un int normal
            snprintf(buffer, sizeof(buffer), "%d%s", l.integer, r.string);

            Value res = eval_makeResultStr(buffer);
            return res;
        }

        switch(node->operator){
            case '+': return eval_makeResultInt(l.integer + r.integer);
            case '-': return eval_makeResultInt(l.integer - r.integer);
            case '*': return eval_makeResultInt(l.integer * r.integer);
            case '/':
            if(r.integer == 0){
                return eval_makeError(error, "Division by 0, execution halted", ERROR_DIVISION_BY_ZERO);
            } else {
                return eval_makeResultInt(l.integer / r.integer);
            }
            case '%': return eval_makeResultInt(l.integer % r.integer);
        }
    }
    if(node->type == NODE_ASSIGN){
        Value val = eval(node->right, error);
        if (error->hasError) return eval_makeError_keepErrInfo(error);

        if (val.type == VALUE_INT) {
            insertInt(node->varName, val.integer);
        } else if (val.type == VALUE_STR) {
            insertStr(node->varName, val.string);
            free(val.string);
        }
        return eval_makeResultVoid();
    }
    if(node->type == NODE_PRINT){
        Value val = eval(node->right, error);
        if (error->hasError) return eval_makeError_keepErrInfo(error);
        
        if (val.type == VALUE_INT) {
            printf("%d\n", val.integer);
        } else if (val.type == VALUE_STR) {
            printf("%s\n", val.string);
            free(val.string);
        }
        return eval_makeResultVoid();
    }
    if(node->type == NODE_RETURN){
        Value val = eval(node->right, error);
        if (error->hasError) return eval_makeError_keepErrInfo(error);
        
        if (val.type == VALUE_INT) {
            eval_return(val.integer); // Sets a flag indicating that the last instruction was a return, used to stop eval()
        } else if (val.type == VALUE_STR) {
            return eval_makeError(error, "Tried to return a string", ERROR_MISC);
        }
    }

    if(node->type == NODE_INCREMENT){
        Value val = eval(node->right, error);

        if(val.type == VALUE_INT) {
            if (error->hasError) return eval_makeError_keepErrInfo(error);

            val.integer++;
            insertInt(node->right->varName, val.integer);
        }
        return eval_makeResultVoid();
    }

    if(node->type == NODE_DECREMENT){
        Value val = eval(node->right, error);

        if(val.type == VALUE_INT) {
            if (error->hasError) return eval_makeError_keepErrInfo(error);

            val.integer--;
            insertInt(node->right->varName, val.integer);
        }
        return eval_makeResultVoid();
    }

    return eval_makeError(error, "Tried to evaluate unknown type node", ERROR_MISC);
}


