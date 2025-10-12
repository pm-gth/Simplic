#include "simplicError.h"
#include <time.h>

SimplicError* initError(void){
        SimplicError* newError = malloc(sizeof(SimplicError));
        newError->hasError = false;
        newError->errMsg = NULL;
        newError->errCode = NO_ERROR;

        return newError;
}

void setError(SimplicError* error, const char* msg, SimplicErrorType errCode){
        if(error == NULL){
                return;
        }

        error->hasError = true;

        // Override error message
        if(error->errMsg != NULL){
                free(error->errMsg);
        }

        int len = strlen(msg);
        error->errMsg = malloc(sizeof(char)*(len+1));
        strcpy(error->errMsg, msg);
        error->errCode = errCode;
}

void printError(SimplicError* error){
        if(error == NULL){
                return;
        }

        printf("\nError: %s\n", error->errMsg);
}

void unsetError(SimplicError* error){
        if(error == NULL){
                return;
        }
        error->hasError = false;
        
        if(error->errMsg != NULL){
                free(error->errMsg);
                error->errMsg = NULL;
        }

        error->errCode = 0;
}

void deleteError(SimplicError** error){
        if(error == NULL){
                return;
        }

        if((*error)->errMsg != NULL){
                free((*error)->errMsg);
        }

        free(*error);
        *error = NULL;
}