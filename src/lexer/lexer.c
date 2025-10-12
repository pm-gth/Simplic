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

Token* initTokenList(){
	return NULL;
}

Token* createToken(TokenType type, const char* name, char* string){
	Token* newToken = malloc(sizeof(Token));

	if(newToken == NULL){
		return NULL;
	}

	newToken->type = type;
	newToken->next = NULL;
	strcpy(newToken->name, name);

	// If it's a string token, store the string literal
	if(string != NULL){
		newToken->string = string;
	} else {
		newToken->string = NULL;
	}

	return newToken;
}

int addTokenToTail(Token** tokenList, TokenType type, const char* name, char* string){
	Token* newToken = createToken(type, name, string);

	if(!newToken){
		return -1;
	}

	// Case: List is empty
	if(*tokenList == NULL){
		*tokenList = newToken;
	}
	// Case: list is not empty
	// Find last token and append
	else{
		Token* curr = *tokenList;
		while(curr->next != NULL){
			curr = curr->next;
		}
		curr->next = newToken;
	}

	return 0;
}

int removeTokenFromHead(Token** tokenList){
	// If it's a string token, remove its content
	if((*tokenList)->string != NULL){
		free((*tokenList)->string);
	}
	
	// Case: List is empty
	if(*tokenList == NULL){
		return -1;
	}

	// Case: List only has one element
	if((*tokenList)->next == NULL){
		free(*tokenList);
		*tokenList = NULL;
	}
	// Case: List has two or more elements
	else{
		Token* oldHead = *tokenList;
		*tokenList = (*tokenList)->next;
		free(oldHead);
	}

	return 0;
}

void printTokens(Token* tokenList){
	Token* curr = tokenList;
	
	while(curr->next != NULL){
		printf("%s ", curr->name);
		curr = curr->next;
	}
	printf("%s\n", curr->name); // Last token
}

int emptyTokenList(Token** tokenList){
	// Case: List is empty
	if(*tokenList == NULL){
		return -1;
	}

	// Case: List is not already empty
	Token* curr = *tokenList;
	Token* oldNext;

	while(curr->next != NULL){
		oldNext = curr->next;
		free(curr);
		curr = oldNext;
	}
	free(curr);
	*tokenList = NULL;

	return 0;
}

void tokenizeSource(Token** tokenList, const char* src, SimplicError* error) {
	unsigned short int i = 0;
	int len = strlen(src);

	while(i < len){
		// Symbol or escape seq
		while (src[i] == ' ' || src[i] == '\t' || src[i] == '\n') i++;

		if (src[i] == '=')  { i++; addTokenToTail(tokenList, TOKEN_EQUALS, "=", NULL); continue; }
		if (src[i] == '+')  { i++; addTokenToTail(tokenList, TOKEN_PLUS, "+", NULL); continue; }
		if (src[i] == '-')  { i++; addTokenToTail(tokenList, TOKEN_MINUS, "-", NULL); continue; }
		if (src[i] == '*')  { i++; addTokenToTail(tokenList, TOKEN_MULT, "*", NULL); continue; }
		if (src[i] == '/')  { i++; addTokenToTail(tokenList, TOKEN_DIV, "/", NULL); continue; }
		if (src[i] == '%')  { i++; addTokenToTail(tokenList, TOKEN_MOD, "%", NULL); continue; }

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
			addTokenToTail(tokenList, TOKEN_STRING, "STRING", buffer); 
			continue;
		}

		// Identifier or variable
		if (isAlpha(src[i])) {
			char buffer[IDENTIFIER_SIZE]; int j = 0;
			while (isAlphaNumer(src[i])) buffer[j++] = src[i++];
			buffer[j] = '\0';
			if (strcmp(buffer, "SET") == 0) { addTokenToTail(tokenList, TOKEN_SET, "SET", NULL); continue; }
			if (strcmp(buffer, "UNSET") == 0) { addTokenToTail(tokenList, TOKEN_UNSET, "UNSET", NULL); continue; }
			if (strcmp(buffer, "PRINT") == 0) { addTokenToTail(tokenList, TOKEN_PRINT, "PRINT", NULL); continue; }
			if (strcmp(buffer, "PRINTLN") == 0) { addTokenToTail(tokenList, TOKEN_PRINTLN, "PRINTLN", NULL); continue; }
			if (strcmp(buffer, "RETURN") == 0) { addTokenToTail(tokenList, TOKEN_RETURN, "RETURN", NULL); continue; }
			if (strcmp(buffer, "INCR") == 0) { addTokenToTail(tokenList, TOKEN_INCREMENT, "INCR", NULL); continue; }
			if (strcmp(buffer, "DECR") == 0) { addTokenToTail(tokenList, TOKEN_DECREMENT, "DECR", NULL); continue; }
			if (strcmp(buffer, "GT") == 0) { addTokenToTail(tokenList, TOKEN_GT, "GT", NULL); continue; }
			if (strcmp(buffer, "LT") == 0) { addTokenToTail(tokenList, TOKEN_LT, "LT", NULL); continue; }
			if (strcmp(buffer, "GEQ") == 0) { addTokenToTail(tokenList, TOKEN_GEQ, "GEQ", NULL); continue; }
			if (strcmp(buffer, "LEQ") == 0) { addTokenToTail(tokenList, TOKEN_LEQ, "LEQ", NULL); continue; }
			if (strcmp(buffer, "EQ") == 0) { addTokenToTail(tokenList, TOKEN_EQ, "EQ", NULL); continue; }
			if (strcmp(buffer, "NEQ") == 0) { addTokenToTail(tokenList, TOKEN_NEQ, "NEQ", NULL); continue; }
			if (strcmp(buffer, "AND") == 0) { addTokenToTail(tokenList, TOKEN_AND, "AND", NULL); continue; }
			if (strcmp(buffer, "OR") == 0) { addTokenToTail(tokenList, TOKEN_OR, "OR", NULL); continue; }
			if (strcmp(buffer, "WHILE") == 0) { addTokenToTail(tokenList, TOKEN_WHILE, "WHILE", NULL); continue; }
			if (strcmp(buffer, "DO") == 0) { addTokenToTail(tokenList, TOKEN_DO, "DO", NULL); continue; }
			if (strcmp(buffer, "DONE") == 0) { addTokenToTail(tokenList, TOKEN_DONE, "DONE", NULL); continue; }
			addTokenToTail(tokenList, TOKEN_VAR, buffer, NULL); continue;
		}

		// Number
		if (isNumber(src[i])) {
			char buffer[IDENTIFIER_SIZE]; int j = 0;
			while (isNumber(src[i])) buffer[j++] = src[i++];
			buffer[j] = '\0';
			addTokenToTail(tokenList, TOKEN_NUMBER, buffer, NULL); continue;
		}

		i++; // Unknown character
		// Maybe create unknown token and halt interpreter ???
	}
	addTokenToTail(tokenList, TOKEN_EOF, "", NULL); // Reached string end, add EOF
}
