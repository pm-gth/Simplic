#include "private_token.h"

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

Token* initTokenQueue(){
	return NULL;
}

int enqueueToken(Token** tokenList, TokenType type, const char* name, char* string){
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

void deleteFirstToken(Token** tokenList) {
	// Case: List is empty
	if(*tokenList == NULL){
		return;
	}

	// Case: List only has one element
	if((*tokenList)->next == NULL){
		freeToken(*tokenList);
		*tokenList = NULL;
	}
	// Case: List has two or more elements
	else{
		Token* oldHead = *tokenList;
		*tokenList = (*tokenList)->next;
		freeToken(oldHead);
	}
}

void freeToken(Token* token) {
	// If it's a string token, remove its content
	if(token->string != NULL){
		free(token->string);
	}
	free(token);
}

void printTokenQueue(Token* tokenList){
	Token* curr = tokenList;
	
	while(curr->next != NULL){
		printf("%s ", curr->name);
		curr = curr->next;
	}
	printf("%s\n", curr->name); // Last token
}

int emptyTokenQueue(Token** tokenList){
	// Case: List is empty
	if(*tokenList == NULL){
		return -1;
	}

	// Case: List is not already empty
	Token* curr = *tokenList;
	Token* oldNext;

	while(curr->next != NULL){
		oldNext = curr->next;
		freeToken(curr);
		curr = oldNext;
	}
	freeToken(curr);
	*tokenList = NULL;

	return 0;
}

Token* peekTokenQueue(Token** tokenList) {
    return *tokenList;
}

Token dequeueToken(Token** tokenList) {
    Token copy;
    copy.type = (*tokenList)->type;
    copy.next = (*tokenList)->next;
    strcpy(copy.name, (*tokenList)->name);

    deleteFirstToken(tokenList);

    return copy;
}