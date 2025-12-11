#ifndef SIMPLICERROR_H
#define SIMPLICERROR_H

/*
=======================================================================================
 Structure used to handle errors in the interpreter. Most modules have interfaces and
 wrappers to throw errors (like ParserResult or Bankresult) which end up calling to
 SimplicError. Simplic error contains a flag which indicates if an exception has been
 thrown, an error message meant to be printed and an error ID which can be used to
 try and recover from the exception
=======================================================================================
*/

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
    ERROR_NON_TERMINATED_BLOCK,
    ERROR_TYPE_MISMATCH,
    ERROR_READING_SCRIPT_FILE,
    ERROR_MALLOC_FAILURE,
    ERROR_SYMBOL_REASIGNATION,
    ERROR_EMPTY_JUMP_STACK,
    ERROR_MISC // Do not bother with these
} SimplicErrorType;

typedef struct SimplicError SimplicError;
struct SimplicError {
    bool hasError;
    char* errMsg;
    SimplicErrorType errCode;
};

SimplicError* initError(void); // Allocates error in memory
void setError(SimplicError* error, SimplicErrorType errCode, const char* format, ...); // Throws error with message
void unsetError(SimplicError* error); // Unsets error
void printError(SimplicError* error);
void deleteError(SimplicError** error); // Deletes error

#endif