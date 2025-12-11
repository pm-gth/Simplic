#ifndef PRIVATE_INTERPRETER_H
#define PRIVATE_INTERPRETER_H

#include "interpreter.h"

// Wrapper functions for eval()
static SimplicValue eval_makeResultInt(int n);
static SimplicValue eval_makeResultStr(char* s);
static SimplicValue eval_makeResultVoid(); // For empty results such as the one from PRINT
static SimplicValue eval_makeError(SimplicError* err, SimplicErrorType code, const char* fmt, ...);
static SimplicValue eval_makeError_keepErrInfo(SimplicError* err);
static SimplicValue eval_returnInt(int n); // Exits from the interpreter
static SimplicValue eval_returnStr(char* s);

#endif