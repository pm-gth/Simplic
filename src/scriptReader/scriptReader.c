#include "scriptReader.h"

const char* readScriptFile(const char* fileName, SimplicError* error) {
    FILE *script = fopen(fileName, "rb");
    if (!script) {
        setError(error, ERROR_READING_SCRIPT_FILE, "Could not open script %s", fileName);
        return NULL;
    }

    // Get file size
    fseek(script, 0, SEEK_END);
    int size = ftell(script);
    rewind(script);

    char *buffer = malloc(size + 1);
    if (!buffer) {
        setError(error, ERROR_MALLOC_FAILURE, "Could not allocate memory for script file");
        fclose(script);
        return NULL;
    }

    // Add EOF
    int read = fread(buffer, 1, size, script);
    buffer[read] = '\0';
    fclose(script);

    const char *res = buffer; // Make const
    return res;
}
