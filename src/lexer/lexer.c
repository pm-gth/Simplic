#include "lexer.h"

const char* src;

bool isAlpha(char c){
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

bool isNumber(char c){
	return (c >= '0' && c <= '9');
}

bool isAlphaNumer(char c){
	return (isAlpha(c) || isNumber(c));
}

void initTokenList(Token** tokenList){
	(*tokenList) = NULL;
}

Token* createToken(TokenType type, const char* text){
	Token* newToken = malloc(sizeof(Token));

	if(newToken == NULL){
		return NULL;
	}

	newToken->type = type;
	newToken->next = NULL;
	strcpy(newToken->text, text);

	return newToken;
}

int addTokenToTail(Token** tokenList, TokenType type, const char* text){
	Token* newToken = createToken(type, text);

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
		if(curr->type == TOKEN_NEWLINE){
			putchar('\n');
		} else {
			printf("%s ", curr->text);
		}
		curr = curr->next;
	}
	printf("%s\n", curr->text); // Last token
}

int removeAllTokens(Token** tokenList){
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

void tokenizeSource(Token** tokenList, const char* src) {
	unsigned short int i = 0;
	int len = strlen(src);

	while(i < len){
		// Symbol or escape seq
		while (src[i] == ' ' || src[i] == '\t') i++;

		if (src[i] == '\n') { i++; addTokenToTail(tokenList, TOKEN_NEWLINE, "\\n"); continue; }
		if (src[i] == '=')  { i++; addTokenToTail(tokenList, TOKEN_EQUALS, "="); continue; }
		if (src[i] == '+')  { i++; addTokenToTail(tokenList, TOKEN_PLUS, "+"); continue; }

		// Identifier or variable
		if (isAlpha(src[i])) {
			char buffer[MAX_TOKEN_TEXT]; int j = 0;
			while (isAlphaNumer(src[i])) buffer[j++] = src[i++];
			buffer[j] = '\0';
			if (strcmp(buffer, "SET") == 0) { addTokenToTail(tokenList, TOKEN_SET, "SET"); continue; }
			if (strcmp(buffer, "PRINT") == 0) { addTokenToTail(tokenList, TOKEN_PRINT, "PRINT"); continue; }
			addTokenToTail(tokenList, TOKEN_VAR, buffer); continue;
		}

		// Number
		if (isNumber(src[i])) {
			char buffer[MAX_TOKEN_TEXT]; int j = 0;
			while (isNumber(src[i])) buffer[j++] = src[i++];
			buffer[j] = '\0';
			addTokenToTail(tokenList, TOKEN_NUMBER, buffer); continue;
		}

		i++; // Unknown character
		// Maybe create unknown token and halt interpreter ???
	}
	addTokenToTail(tokenList, TOKEN_EOF, ""); // Reached string end, add EOF
}
