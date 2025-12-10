#include "lexer.h"
#include "private_lexer.h"
#include "simplicError.h"

const char* src;

// TODO: Support '_' for variable names
bool isAlpha(char c){
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

bool isNumber(char c){
	return (c >= '0' && c <= '9');
}

bool isAlphaNumer(char c){
	return (isAlpha(c) || isNumber(c));
}

void tokenizeSource(Token** tokenList, const char* src, SimplicError* error) {
	unsigned short int i = 0;
	int len = strlen(src);

	while(i < len){
		// Symbol or escape seq
		while (src[i] == ' ' || src[i] == '\t' || src[i] == '\n') i++;

		// Coment '#', skip to the next \n (or EOF)
		if(src[i] == '#') { while(src[i] != '\n' && i < len) i++; }

		if (src[i] == '=')  { i++; enqueueToken(tokenList, TOKEN_EQUALS, "=", NULL); continue; }
		if (src[i] == '+')  { i++; enqueueToken(tokenList, TOKEN_PLUS, "+", NULL); continue; }
		if (src[i] == '-')  { i++; enqueueToken(tokenList, TOKEN_MINUS, "-", NULL); continue; }
		if (src[i] == '*')  { i++; enqueueToken(tokenList, TOKEN_MULT, "*", NULL); continue; }
		if (src[i] == '/')  { i++; enqueueToken(tokenList, TOKEN_DIV, "/", NULL); continue; }
		if (src[i] == '%')  { i++; enqueueToken(tokenList, TOKEN_MOD, "%", NULL); continue; }

		// String literal
		if (src[i] == '"') { 
			i++; // Skip ' " '
			int j = i;
			while(j < len && src[j] != '"') { 
				j++; 
			}

			if(j >= len){
				setError(error, ERROR_NON_TERMINATED_STRING_LITERAL, "Could not find end of string literal");
				return;
			}

			// String is correctly delimited
			char* buffer = malloc(sizeof(char)*(j - i +1));
			j = 0;

			while(src[i] != '"') { 
				buffer[j] = src[i];
				j++; i++;
			}
			buffer[j] = '\0';

			i++; // Skip last ' " '
			enqueueToken(tokenList, TOKEN_STRING, "STRING", buffer); 
			continue;
		}

		// Identifier or variable
		if (isAlpha(src[i])) {
			char buffer[IDENTIFIER_SIZE]; int j = 0;
			while (isAlphaNumer(src[i])) buffer[j++] = src[i++];
			buffer[j] = '\0';
			if (strcmp(buffer, "SET") == 0) { enqueueToken(tokenList, TOKEN_SET, "SET", NULL); continue; }
			if (strcmp(buffer, "UNSET") == 0) { enqueueToken(tokenList, TOKEN_UNSET, "UNSET", NULL); continue; }
			if (strcmp(buffer, "PRINT") == 0) { enqueueToken(tokenList, TOKEN_PRINT, "PRINT", NULL); continue; }
			if (strcmp(buffer, "PRINTLN") == 0) { enqueueToken(tokenList, TOKEN_PRINTLN, "PRINTLN", NULL); continue; }
			if (strcmp(buffer, "RETURN") == 0) { enqueueToken(tokenList, TOKEN_RETURN, "RETURN", NULL); continue; }
			if (strcmp(buffer, "INCR") == 0) { enqueueToken(tokenList, TOKEN_INCREMENT, "INCR", NULL); continue; }
			if (strcmp(buffer, "DECR") == 0) { enqueueToken(tokenList, TOKEN_DECREMENT, "DECR", NULL); continue; }
			if (strcmp(buffer, "GT") == 0) { enqueueToken(tokenList, TOKEN_GT, "GT", NULL); continue; }
			if (strcmp(buffer, "LT") == 0) { enqueueToken(tokenList, TOKEN_LT, "LT", NULL); continue; }
			if (strcmp(buffer, "GEQ") == 0) { enqueueToken(tokenList, TOKEN_GEQ, "GEQ", NULL); continue; }
			if (strcmp(buffer, "LEQ") == 0) { enqueueToken(tokenList, TOKEN_LEQ, "LEQ", NULL); continue; }
			if (strcmp(buffer, "EQ") == 0) { enqueueToken(tokenList, TOKEN_EQ, "EQ", NULL); continue; }
			if (strcmp(buffer, "NEQ") == 0) { enqueueToken(tokenList, TOKEN_NEQ, "NEQ", NULL); continue; }
			if (strcmp(buffer, "AND") == 0) { enqueueToken(tokenList, TOKEN_AND, "AND", NULL); continue; }
			if (strcmp(buffer, "OR") == 0) { enqueueToken(tokenList, TOKEN_OR, "OR", NULL); continue; }
			if (strcmp(buffer, "WHILE") == 0) { enqueueToken(tokenList, TOKEN_WHILE, "WHILE", NULL); continue; }
			if (strcmp(buffer, "DO") == 0) { enqueueToken(tokenList, TOKEN_DO, "DO", NULL); continue; }
			if (strcmp(buffer, "DONE") == 0) { enqueueToken(tokenList, TOKEN_DONE, "DONE", NULL); continue; }
			if (strcmp(buffer, "IF") == 0) { enqueueToken(tokenList, TOKEN_IF, "IF", NULL); continue; }
			if (strcmp(buffer, "THEN") == 0) { enqueueToken(tokenList, TOKEN_THEN, "THEN", NULL); continue; }
			if (strcmp(buffer, "ELSE") == 0) { enqueueToken(tokenList, TOKEN_ELSE, "ELSE", NULL); continue; }
			if (strcmp(buffer, "FI") == 0) { enqueueToken(tokenList, TOKEN_FI, "FI", NULL); continue; }
			enqueueToken(tokenList, TOKEN_VAR, buffer, NULL); continue;
		}

		// Number
		if (isNumber(src[i])) {
			char buffer[IDENTIFIER_SIZE]; int j = 0;
			while (isNumber(src[i])) buffer[j++] = src[i++];
			buffer[j] = '\0';
			enqueueToken(tokenList, TOKEN_NUMBER, buffer, NULL); continue;
		}

		i++; // Unknown character
		// Maybe create unknown token and halt interpreter ???
	}
	enqueueToken(tokenList, TOKEN_EOF, "", NULL); // Reached string end, add EOF
}
