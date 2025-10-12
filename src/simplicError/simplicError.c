#include "simplicError.h"
#include <stdarg.h>

SimplicError* initError(void){
        SimplicError* newError = malloc(sizeof(SimplicError));
        newError->hasError = false;
        newError->errMsg = NULL;
        newError->errCode = NO_ERROR;

        return newError;
}

void setError(SimplicError* error, SimplicErrorType errCode, const char* format, ...) {
    if (error == NULL) return;

    error->hasError = true;

    // Liberar mensaje anterior
    if (error->errMsg != NULL) {
        free(error->errMsg);
        error->errMsg = NULL;
    }

    // Construir mensaje con args variables
    va_list args;
    va_start(args, format);

    // Primero calculamos el tamaño necesario
    int len = vsnprintf(NULL, 0, format, args);
    va_end(args);

    if (len < 0) return; // error en vsnprintf

    error->errMsg = malloc(len + 1); // +1 para el '\0'
    if (error->errMsg == NULL) return; // fallo malloc

    va_start(args, format);
    vsnprintf(error->errMsg, len + 1, format, args);
    va_end(args);

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