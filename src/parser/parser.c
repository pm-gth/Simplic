#include "parser.h"
#include "lexer.h"
#include <string.h>

Token* tokenList = NULL;

Token* peek(){
    return tokenList;
}

Token advance(){
    Token copy;
    copy.type = tokenList->type;
    copy.next = tokenList->next;
    strcpy(copy.text, tokenList->text);

    removeTokenFromHead(&tokenList);

    return copy;
}

void freeSyntaxTree(SyntaxNode* tree) {
    if (tree == NULL)
        return;

    switch (tree->type) {
        case NODE_NUMBER:
        case NODE_VAR:
            // No child nodes to free
            break;

        case NODE_BIN_OP:
            freeSyntaxTree(tree->left);
            freeSyntaxTree(tree->right);
            break;

        case NODE_ASSIGN:
        case NODE_PRINT:
        case NODE_RETURN:
        case NODE_INCREMENT:
        case NODE_DECREMENT:
            // These have only one child on the right
            freeSyntaxTree(tree->right);
            break;

        default:
            // Undefined, try to free both branches
            freeSyntaxTree(tree->left);
            freeSyntaxTree(tree->right);
            break;
    }

    free(tree);
}

bool compareSyntaxTree(SyntaxNode* a, SyntaxNode* b) {
    if (a == NULL && b == NULL) return true;
    if (a == NULL || b == NULL) return false;

    if(a->type != b->type)
        return false;

    switch (a->type) {
        case NODE_NUMBER:
            return a->numberValue == b->numberValue;

        case NODE_VAR:
            // Vars are only compared by name, their value is in the bank
            return strcmp(a->varName, b->varName) == 0;

        case NODE_BIN_OP:
            return compareSyntaxTree(a->left, b->left) && compareSyntaxTree(a->right, b->right);

        case NODE_ASSIGN:
        case NODE_PRINT:
        case NODE_RETURN:
        case NODE_INCREMENT:
        case NODE_DECREMENT:
            return compareSyntaxTree(a->right, b->right);

        default:
            // Undefined, try to compare both branches
            return compareSyntaxTree(a->left, b->left) && compareSyntaxTree(a->right, b->right);
    }

    return false;
}

ParseResult makeResult(SyntaxNode* n) {
    return (ParseResult){ .node = n, .hasError = false };
}

ParseResult makeError(SimplicError* err, const char* msg, int code) {
    setError(err, msg, code);
    return (ParseResult){ .node = NULL, .hasError = true };
}

ParseResult parseStatement(SimplicError* error) {
    Token* t = peek();

    if (t->type == TOKEN_NEWLINE) {
        advance();
        return parseStatement(error);
    }

    // Set node contain the target's var name and a right branch with its new value (in form of factor or expression)
    if (t->type == TOKEN_SET) {
        advance(); // consume SET
        Token var = advance(); // variable name
        advance(); // consume '='

        // TODO: CHECK FOR DECLARATION WITHOUT INITIALIZATION

        ParseResult expr = parseExpr(error);
        if (expr.hasError || !expr.node)
            return makeError(error, "Invalid expression in SET statement", ERROR_INVALID_EXPR);

        SyntaxNode* n = malloc(sizeof(SyntaxNode));
        n->type = NODE_ASSIGN;
        strcpy(n->varName, var.text);
        n->right = expr.node;
        return makeResult(n);
    }

    // Print node contains a right branch with the value to be printed
    if (t->type == TOKEN_PRINT) {
        advance(); // consume PRINT
        ParseResult expr = parseExpr(error);
        if (expr.hasError || !expr.node)
            return makeError(error, "Invalid PRINT expression", ERROR_INVALID_EXPR);

        SyntaxNode* n = malloc(sizeof(SyntaxNode));
        n->type = NODE_PRINT;
        n->right = expr.node;

        return makeResult(n);
    }

    // Print node contains a right branch with the value to be returned
    if (t->type == TOKEN_RETURN) {
        advance(); // consume RETURN
        ParseResult expr = parseExpr(error);
        if (expr.hasError || !expr.node)
            return makeError(error, "Invalid RETURN expression", ERROR_INVALID_EXPR);

        SyntaxNode* n = malloc(sizeof(SyntaxNode));
        n->type = NODE_RETURN;
        n->right = expr.node;
        return makeResult(n);
    }

    // Incr/Decr node contains a right branch with the name of the variable to be increased/decreased
    if (t->type == TOKEN_INCREMENT || t->type == TOKEN_DECREMENT) {
        TokenType oldType = t->type;
        advance();

        ParseResult expr = parseExpr(error);
        if (expr.hasError || !expr.node)
            return makeError(error, "Invalid expression in INCR/DECR statement", ERROR_INVALID_EXPR);

        SyntaxNode* n = malloc(sizeof(SyntaxNode));
        n->type = (oldType == TOKEN_INCREMENT)? NODE_INCREMENT : NODE_DECREMENT;
        n->right = expr.node;

        return makeResult(n);
    }

    if (t->type == TOKEN_EOF) {
        return (ParseResult){ .node = NULL, .hasError = false };
    }

    return makeError(error, "Unknown statement", ERROR_UNKNOWN_INSTRUCTION);
}

ParseResult parseFactor(SimplicError* error) {
    Token* t = peek();

    // Number node contains its value
    if (t->type == TOKEN_NUMBER) {
        SyntaxNode* n = malloc(sizeof(SyntaxNode));
        n->type = NODE_NUMBER;
        n->numberValue = atoi(t->text);
        advance();
        return makeResult(n);
    } 

    // Variable node contains its name, used for reference in the var bank
    else if (t->type == TOKEN_VAR) {
        SyntaxNode* n = malloc(sizeof(SyntaxNode));
        n->type = NODE_VAR;
        strcpy(n->varName, t->text);
        advance();
        return makeResult(n);
    }

    return makeError(error, "Expected number or variable", ERROR_UNEXPECTED_TOKEN);
}

ParseResult parseTerm(SimplicError* error) {
    ParseResult left = parseFactor(error);
    if (left.hasError) return left;

    // Binary operation contains an operator (which indicates the operation) and two branches for its both operands
    // first one is in the left child, second's in the right one
    while (peek()->type == TOKEN_MULT || peek()->type == TOKEN_DIV || peek()->type == TOKEN_MOD) {
        TokenType oldType = peek()->type; // Save ops type, we need to advance in the list for the second operand

        advance();
        ParseResult right = parseFactor(error);

        if (right.hasError || !right.node)
            return makeError(error, "Invalid right operand in binary expression", ERROR_UNDEFINED_SECOND_OPERAND);

        SyntaxNode* n = malloc(sizeof(SyntaxNode));
        n->type = NODE_BIN_OP;
        
        switch (oldType) {
            case TOKEN_MULT:
                n->operator = '*';
            case TOKEN_DIV:
                n->operator = '/';
            case TOKEN_MOD:
                n->operator = '%';
            default:
            ; // shut up compiler
        }

        n->left = left.node;
        n->right = right.node;

        left.node = n;
    }

    return left;
}

ParseResult parseExpr(SimplicError* error) {
    ParseResult left = parseTerm(error);
    if (left.hasError) return left;

    // Binary operation contains an operator (which indicates the operation) and two branches for its both operands
    // first one is in the left child, second's in the right one
    while (peek()->type == TOKEN_PLUS || peek()->type == TOKEN_MINUS) {
        TokenType oldType = peek()->type;

        advance();
        ParseResult right = parseTerm(error);

        if (right.hasError || !right.node)
            return makeError(error, "Invalid right operand in expression", ERROR_UNDEFINED_SECOND_OPERAND);

        SyntaxNode* n = malloc(sizeof(SyntaxNode));
        n->type = NODE_BIN_OP;
        n->operator = (oldType == TOKEN_PLUS ? '+' : '-');
        n->left = left.node;
        n->right = right.node;

        left.node = n;
    }

    return left;
}