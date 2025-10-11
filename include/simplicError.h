#ifndef SIMPLICERROR_H
#define SIMPLICERROR_H

#include "simplic.h"

typedef enum {
    NO_ERROR,
    ERROR_DIVISION_BY_ZERO,
    ERROR_UNDEFINED_SECOND_OPERAND,
    ERROR_UNKNOWN_INSTRUCTION,
    ERROR_INVALID_EXPR,
    ERROR_UNEXPECTED_TOKEN,
    ERROR_ACCESS_TO_UNDECLARED_VAR,
    ERROR_NON_TERMINATED_STRING_LITERAL,
    ERROR_MISC // Do not bother with these
} SimplicErrorType;

typedef struct SimplicError SimplicError;
struct SimplicError {
    bool hasError;
    char* errMsg;
    SimplicErrorType errCode;
};

SimplicError* initError(void); // Allocates error in memory
void setError(SimplicError* error, const char* msg, SimplicErrorType errCode); // Throws error with message
void unsetError(SimplicError* error); // Unsets error
void printError(SimplicError* error);
void deleteError(SimplicError** error); // Deletes and deallocates error

#endif