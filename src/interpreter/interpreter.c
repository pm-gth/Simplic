#include "interpreter.h"
#include "parser.h"

void initHashTable(){
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hashTable[i] = NULL;
    }
}

unsigned long stringHash(const char *str){
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash % HASH_TABLE_SIZE;
}

void insertInt(const char* key, int value) {
    unsigned int index = stringHash(key);
    MemoryCell* current = hashTable[index];

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
    newMemCell->next = hashTable[index]; // In case there is a collision
    hashTable[index] = newMemCell;
}

void insertStr(const char* key, const char* str) {
    unsigned int index = stringHash(key);
    MemoryCell* current = hashTable[index];

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

    newMemCell->next = hashTable[index];  // In case there is a collision
    newMemCell->value = -1;
    hashTable[index] = newMemCell;
}


int getInt(const char* key) {
    unsigned int index = stringHash(key);
    MemoryCell* current = hashTable[index];
    while (current != NULL) {
        if (strcmp(current->name, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return -1;
}

char* getStr(const char* key) {
    unsigned int index = stringHash(key);
    MemoryCell* current = hashTable[index];
    while (current != NULL) {
        if (strcmp(current->name, key) == 0) {
            return current->strPtr;
        }
        current = current->next;
    }
    return NULL;
}

int delete(const char* key) {
    unsigned int index = stringHash(key);
    MemoryCell* current = hashTable[index];
    MemoryCell* prev = NULL;
    while (current != NULL) {
        if (strcmp(current->name, key) == 0) {
            if (prev == NULL){
                hashTable[index] = current->next;
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

int eval(SyntaxNode* node, SimplicError* error){
    if(error->hasError) return 0;
    if(node->type == NODE_NUMBER) return node->numberValue;
    if(node->type == NODE_VAR) return getInt(node->varName);
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
        int val = getInt(node->right->varName);
        if(!error->hasError){
            val++;
            insertInt(node->right->varName, val);
        }
    }

    if(node->type == NODE_DECREMENT){
        int val = getInt(node->right->varName);
        if(!error->hasError){
            val--;
            insertInt(node->right->varName, val);
        }
    }

    return 0;
}

int main(void) {
    const char* code =
        "PRINT 1\n"
        "SET X = 4\n"
        "PRINT X\n"
        "INCR X\n"
        "PRINT X"
        ;

    initHashTable();
    initTokenList(&tokenList);
    tokenizeSource(&tokenList, code);

    SimplicError* error = initError();

    for (;;) {
        ParseResult result = parseStatement(error);

        if (!result.node && !result.hasError)
            break;

        if (result.hasError) {
            printError(error);
            break;
        }

        eval(result.node, error);

        if (error->hasError) {
            printError(error);
            break;
        }

        freeSyntaxTree(result.node);
    }

    removeAllTokens(&tokenList);
    return 0;
}


