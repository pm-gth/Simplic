#ifndef SCRIPTREADER_H
#define SCRIPTREADER_H

#include "simplicError.h"
#include "simplic.h"

// Used to read a simplic script from a file
const char* readScriptFile(const char* fileName, SimplicError* error);

#endif