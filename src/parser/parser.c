#include "parser.h"
#include "private_parser.h"
#include "dataStructures/token.h"
#include "simplicError.h"

TokenType findIfBlockDelimiter(Token** tokenList) {
    Token* curr = *tokenList;
    int depth = 0;

    while (curr != NULL) {
        if (curr->type == TOKEN_IF) {
            depth++;
        } else if (curr->type == TOKEN_FI) {
            if (depth == 0) {
                return TOKEN_FI;
            } else {
                depth--;
            }
        } else if (curr->type == TOKEN_ELSE && depth == 0) {
            return TOKEN_ELSE;
        }
        curr = curr->next;
    }
    return TOKEN_ERROR_TOKEN;
}


static SyntaxNode* initNode() {
    SyntaxNode* res = malloc(sizeof(SyntaxNode));
    res->numberValue = 0;
    strcpy(res->operator, "&");
    res->string = NULL;
    res->subnodeA = NULL;
    res->subnodeB = NULL;
    res->subnodeC = NULL;
    res->blockStatements = NULL;
    res->type = 0;

    return res;
}

void freeSyntaxTree(SyntaxNode* tree) {
    int i;

    if (tree == NULL)
        return;

    switch (tree->type) {
        case NODE_NUMBER:
        case NODE_VAR:
        case NODE_UNASSIGN:
            // No child nodes to free
            break;
        case NODE_STRING:
            free(tree->string);
            break;

        case NODE_BIN_OP:
        case NODE_WHILE:
            freeSyntaxTree(tree->subnodeA);
            freeSyntaxTree(tree->subnodeB);
            break;

        case NODE_BLOCK: // Array of ASTs
            i = 0;
            while (tree->blockStatements[i] != NULL) { // Free each AST
                freeSyntaxTree(tree->blockStatements[i++]);
            }
            free(tree->blockStatements); // Free list of ASts
            break;

        case NODE_IF:
            freeSyntaxTree(tree->subnodeA);
            freeSyntaxTree(tree->subnodeB);
            freeSyntaxTree(tree->subnodeC);
            break;

        case NODE_ASSIGN:
        case NODE_PRINT:
        case NODE_PRINTLN:
        case NODE_RETURN:
        case NODE_INCREMENT:
        case NODE_DECREMENT:
            // These have only one child on the right
            freeSyntaxTree(tree->subnodeB);
            break;

        default:
            // Undefined, try to free both branches
            freeSyntaxTree(tree->subnodeA);
            freeSyntaxTree(tree->subnodeB);
            break;
    }

    free(tree);
}

bool compareSyntaxTree(SyntaxNode* a, SyntaxNode* b) {
    int i;
    if (a == NULL && b == NULL) return true;
    if (a == NULL || b == NULL) return false;

    if(a->type != b->type)
        return false;

    switch (a->type) {
        case NODE_NUMBER:
            return a->numberValue == b->numberValue;

        case NODE_VAR:
        case NODE_UNASSIGN:
            // Vars are only compared by name, their value is in the bank
            return strcmp(a->varName, b->varName) == 0;

        case NODE_STRING:
            // Compare string contents
            return strcmp(a->string, b->string) == 0;

        case NODE_BIN_OP:
        case NODE_WHILE:
            return compareSyntaxTree(a->subnodeA, b->subnodeA) && compareSyntaxTree(a->subnodeB, b->subnodeB);

        case NODE_BLOCK:
            i = 0;
            while (a->blockStatements[i] != NULL && b->blockStatements[i] != NULL) {
                if (!compareSyntaxTree(a->blockStatements[i], b->blockStatements[i]))
                    return false;
                i++;
                }

            // One is larger than the other
            if (a->blockStatements[i] != NULL || b->blockStatements[i] != NULL)
                return false;
            return true;

        case NODE_IF:
            return compareSyntaxTree(a->subnodeA, b->subnodeA) && compareSyntaxTree(a->subnodeB, b->subnodeB) && compareSyntaxTree(a->subnodeC, b->subnodeC);

        case NODE_ASSIGN:
        case NODE_PRINT:
        case NODE_PRINTLN:
        case NODE_RETURN:
        case NODE_INCREMENT:
        case NODE_DECREMENT:
            return compareSyntaxTree(a->subnodeB, b->subnodeB);

        default:
            // Undefined, try to compare both branches
            return compareSyntaxTree(a->subnodeA, b->subnodeA) && compareSyntaxTree(a->subnodeB, b->subnodeB);
    }

    return false;
}

ParseResult makeResult(SyntaxNode* n) {
    return (ParseResult){ .node = n, .hasError = false };
}

ParseResult makeError(SimplicError* err, SimplicErrorType code, const char* fmt, ...) {
    if (err != NULL) {
        va_list args;
        va_start(args, fmt);

        // msg len
        int len = vsnprintf(NULL, 0, fmt, args);
        va_end(args);

        if (len >= 0) {
            char* buffer = malloc(len + 1);
            if (buffer != NULL) {
                va_start(args, fmt);
                vsnprintf(buffer, len + 1, fmt, args);
                va_end(args);

                setError(err, code, "%s", buffer);
                free(buffer);
            }
        }
    }

    return (ParseResult){ .node = NULL, .hasError = true };
}

ParseResult makeError_keepErrInfo(SimplicError* err) {
    return makeError(err, err->errCode, err->errMsg);
}

ParseResult parseStatement(Token** tokenList, SimplicError* error) {
    Token* t = peekTokenQueue(tokenList);

    if (!t) return makeError(error, ERROR_UNKNOWN_INSTRUCTION, "Unexpected end of token list");

    // Block end, return NULL
    if (t->type == TOKEN_DONE) {
        return (ParseResult){ .node = NULL, .hasError = false };
    }

    if (t->type == TOKEN_FI) {
        return (ParseResult){ .node = NULL, .hasError = false };
    }

    if (t->type == TOKEN_ELSE) {
        return (ParseResult){ .node = NULL, .hasError = false };
    }

    if (t->type == TOKEN_EOF) {
        return (ParseResult){ .node = NULL, .hasError = false };
    }

    // ------------------------------------------
    // SET Node -> Variable name
    // Subnode B: new value for the variable 
    // ------------------------------------------
    if (t->type == TOKEN_SET) {
        dequeueToken(tokenList); // consume SET
        Token var = dequeueToken(tokenList); // variable name

        SyntaxNode* valueNode;
        if(peekTokenQueue(tokenList)->type != TOKEN_EQUALS){
            // The variable is only being declared
            // Create a number node with 0 to initilize
            valueNode = initNode();
            valueNode->type = NODE_NUMBER;
            valueNode->numberValue = 0;
        } else {
            dequeueToken(tokenList); // consume '='
            ParseResult expr = parseLowestPrecedenceOperation(tokenList, error);
            if (expr.hasError || !expr.node)
                return makeError(error, ERROR_INVALID_EXPR, "Invalid expression in SET statement");
            valueNode = expr.node;
        }

        SyntaxNode* n = initNode();
        n->type = NODE_ASSIGN;
        strcpy(n->varName, var.name);
        n->subnodeB = valueNode; // Var's value
        return makeResult(n);
    }

    // ------------------------------------------
    // UNSET Node -> Variable name
    // ------------------------------------------
    if (t->type == TOKEN_UNSET) {
        dequeueToken(tokenList); // consume UNSET
        Token var = dequeueToken(tokenList); // variable name
        SyntaxNode* n = initNode();
        n->type = NODE_UNASSIGN;
        strcpy(n->varName, var.name);
        return makeResult(n);
    }

    // ------------------------------------------
    // PRINT Node
    // Subnode B: value to be printed
    // ------------------------------------------
    if (t->type == TOKEN_PRINT || t->type == TOKEN_PRINTLN) {
        TokenType oldType = t->type;
        dequeueToken(tokenList); // consume PRINT
        ParseResult expr = parseLowestPrecedenceOperation(tokenList, error);
        if (expr.hasError || !expr.node)
            return makeError(error, ERROR_INVALID_EXPR, "Invalid PRINT expression");
        SyntaxNode* n = initNode();
        n->type = (oldType == TOKEN_PRINT) ? NODE_PRINT : NODE_PRINTLN;
        n->subnodeB = expr.node;
        return makeResult(n);
    }

    // ------------------------------------------
    // RETURN Node
    // Subnode B: value to be returned
    // ------------------------------------------
    if (t->type == TOKEN_RETURN) {
        dequeueToken(tokenList); // consume RETURN
        ParseResult expr = parseLowestPrecedenceOperation(tokenList, error);
        if (expr.hasError || !expr.node)
            return makeError(error, ERROR_INVALID_EXPR, "Invalid RETURN expression");
        SyntaxNode* n = initNode();
        n->type = NODE_RETURN;
        n->subnodeB = expr.node;
        return makeResult(n);
    }

    // ------------------------------------------
    // INCR/DECR Node
    // Subnode B: name of variable to be modified
    // ------------------------------------------
    if (t->type == TOKEN_INCREMENT || t->type == TOKEN_DECREMENT) {
        TokenType oldType = t->type;
        dequeueToken(tokenList);
        ParseResult expr = parseLowestPrecedenceOperation(tokenList, error);
        if (expr.hasError || !expr.node)
            return makeError(error, ERROR_INVALID_EXPR, "Invalid expression in INCR/DECR statement");
        SyntaxNode* n = initNode();
        n->type = (oldType == TOKEN_INCREMENT) ? NODE_INCREMENT : NODE_DECREMENT;
        n->subnodeB = expr.node;
        return makeResult(n);
    }

    // ------------------------------------------
    // WHILE Node
    // Subnode A: loop condition
    // Subnode B: block of code
    // ------------------------------------------
    if (t->type == TOKEN_WHILE) {
        dequeueToken(tokenList); // consume WHILE
        // Condition
        ParseResult cond = parseLowestPrecedenceOperation(tokenList, error);
        if (cond.hasError || !cond.node)
            return makeError_keepErrInfo(error);

        if (peekTokenQueue(tokenList)->type != TOKEN_DO)
            return makeError(error, ERROR_MISC, "WHILE missing DO keyword, instead recived %s", peekTokenQueue(tokenList)->name);
        dequeueToken(tokenList); // consume DO

        // Body (block)
        SyntaxNode* body = parseBlock(tokenList, error, TOKEN_DONE);
        
        if (error->hasError || !body)
            makeError_keepErrInfo(error);

        SyntaxNode* n = initNode();
        n->type = NODE_WHILE;
        n->subnodeA = cond.node;
        n->subnodeB = body;
        return makeResult(n);
    }

    // ------------------------------------------
    // IF Node
    // Subnode A: if condition
    // Subnode B: if code block
    // Subnode C: NULL or else code block
    // ------------------------------------------
    if (t->type == TOKEN_IF) {
        dequeueToken(tokenList); // consume IF
        // Condition
        ParseResult cond = parseLowestPrecedenceOperation(tokenList, error);
        if (cond.hasError || !cond.node)
            return makeError_keepErrInfo(error);

        if (peekTokenQueue(tokenList)->type != TOKEN_THEN)
            return makeError(error, ERROR_MISC, "IF missing THEN keyword, instead recived %s", peekTokenQueue(tokenList)->name);
        dequeueToken(tokenList); // consume THEN

        // Determine if block delimiter is FI or ELSE
        TokenType blockDelimiter = findIfBlockDelimiter(tokenList);
        if(blockDelimiter == TOKEN_ERROR_TOKEN)
            return makeError(error, ERROR_NON_TERMINATED_BLOCK, "IF missing delimiter keyword");

        // If body, will be executed if condition is true
        SyntaxNode* ifBody = parseBlock(tokenList, error, blockDelimiter);

        if (error->hasError || !ifBody)
            makeError_keepErrInfo(error);

        // If delimiter was else, we store a second body that will be executed if condition is false
        SyntaxNode* elseBody = NULL;
        if(blockDelimiter == TOKEN_ELSE) {
            elseBody = parseBlock(tokenList, error, TOKEN_FI);
            if (error->hasError || !elseBody)
                makeError_keepErrInfo(error);
        }

        SyntaxNode* n = initNode();
        n->type = NODE_IF;
        n->subnodeA = cond.node;
        n->subnodeB = ifBody;
        n->subnodeC = elseBody;
        return makeResult(n);
    }

    return makeError(error, ERROR_UNKNOWN_INSTRUCTION, "Unknown statement: %s", t->name);
}


ParseResult parseFactor(Token** tokenList, SimplicError* error) {
    Token* t = peekTokenQueue(tokenList);

    // ------------------------------------------
    // NUMBER Node -> its value
    // ------------------------------------------
    if (t->type == TOKEN_NUMBER) {
        SyntaxNode* n = initNode();
        n->type = NODE_NUMBER;
        n->numberValue = atoi(t->name);
        dequeueToken(tokenList);
        return makeResult(n);
    } 

    // ------------------------------------------
    // VARIABLE Node -> its name
    // ------------------------------------------
    else if (t->type == TOKEN_VAR) {
        SyntaxNode* n = initNode();
        n->type = NODE_VAR;
        strcpy(n->varName, t->name);
        dequeueToken(tokenList);
        return makeResult(n);
    }

    // ------------------------------------------
    // STRING Node -> its text
    // ------------------------------------------
    else if (t->type == TOKEN_STRING) {
        SyntaxNode* n = initNode();
        n->type = NODE_STRING;
        n->string = malloc(sizeof(char)*(strlen(t->string)+1));
        n->numberValue = 0;
        strcpy(n->string, t->string);
        dequeueToken(tokenList);
        return makeResult(n);
    }

    return makeError(error, ERROR_UNEXPECTED_TOKEN, "Expected number or variable, instead received: %s", t->name);
}

ParseResult parseTerm(Token** tokenList, SimplicError* error) {
    ParseResult left = parseFactor(tokenList, error);
    if (left.hasError) return left;

    // ------------------------------------------
    // BINARY_OP Node -> operator
    // Subnode A: first operand
    // Subnode B: second operand
    // ------------------------------------------
    while (peekTokenQueue(tokenList)->type == TOKEN_MULT || peekTokenQueue(tokenList)->type == TOKEN_DIV || peekTokenQueue(tokenList)->type == TOKEN_MOD) {
        TokenType oldType = peekTokenQueue(tokenList)->type; // Save ops type, we need to advance in the list for the second operand

        dequeueToken(tokenList);
        ParseResult right = parseFactor(tokenList, error);

        if (right.hasError || !right.node)
            return makeError(error, ERROR_UNDEFINED_SECOND_OPERAND, "Invalid right operand in binary term");

        SyntaxNode* n = initNode();
        n->type = NODE_BIN_OP;
        
        switch (oldType) {
            case TOKEN_MULT:
                strcpy( n->operator, "*");
                break;
            case TOKEN_DIV:
                strcpy( n->operator, "/");
                break;
            case TOKEN_MOD:
                strcpy( n->operator, "%");
                break;
            default:
            ; // shut up the compiler
        }

        n->subnodeA = left.node;
        n->subnodeB = right.node;

        left.node = n;
    }

    return left;
}

ParseResult parseExpr(Token** tokenList, SimplicError* error) {
    ParseResult left = parseTerm(tokenList, error);
    if (left.hasError) return left;

    // ------------------------------------------
    // BINARY_OP Node -> operator
    // Subnode A: first operand
    // Subnode B: second operand
    // ------------------------------------------
    while (peekTokenQueue(tokenList)->type == TOKEN_PLUS || peekTokenQueue(tokenList)->type == TOKEN_MINUS) {
        TokenType oldType = peekTokenQueue(tokenList)->type;

        dequeueToken(tokenList);
        ParseResult right = parseTerm(tokenList, error);

        if (right.hasError || !right.node)
            return makeError(error, ERROR_UNDEFINED_SECOND_OPERAND, "Invalid right operand in expression");

        SyntaxNode* n = initNode();
        n->type = NODE_BIN_OP;
        switch(oldType){
            case TOKEN_PLUS:
                strcpy( n->operator, "+");
                break;
            case TOKEN_MINUS:
                strcpy( n->operator, "-");
                break;
            default:
            ;
        }
        n->subnodeA = left.node;
        n->subnodeB = right.node;

        left.node = n;
    }

    return left;
}

ParseResult parseRelational(Token** tokenList, SimplicError* error) {
    ParseResult left = parseExpr(tokenList, error);
    if (left.hasError) return left;

    // ------------------------------------------
    // BINARY_OP Node -> operator
    // Subnode A: first operand
    // Subnode B: second operand
    // ------------------------------------------
    while (peekTokenQueue(tokenList)->type == TOKEN_GT || peekTokenQueue(tokenList)->type == TOKEN_GEQ || peekTokenQueue(tokenList)->type == TOKEN_LT || peekTokenQueue(tokenList)->type == TOKEN_LEQ) {
        TokenType oldType = peekTokenQueue(tokenList)->type;

        dequeueToken(tokenList);
        ParseResult right = parseTerm(tokenList, error);

        if (right.hasError || !right.node)
            return makeError(error, ERROR_UNDEFINED_SECOND_OPERAND, "Invalid right operand in relational comparison");

        SyntaxNode* n = initNode();
        n->type = NODE_BIN_OP;
        switch(oldType){
            case TOKEN_GT:
                strcpy( n->operator, ">");
                break;
            case TOKEN_GEQ:
                strcpy( n->operator, ">=");
                break;
            case TOKEN_LT:
                strcpy( n->operator, "<");
                break;
            case TOKEN_LEQ:
                strcpy( n->operator, "<=");
                break;
            default:
            ;
        }
        n->subnodeA = left.node;
        n->subnodeB = right.node;

        left.node = n;
    }

    return left;
}

ParseResult parseEquality(Token** tokenList, SimplicError* error) {
    ParseResult left = parseRelational(tokenList, error);
    if (left.hasError) return left;

    // ------------------------------------------
    // BINARY_OP Node -> operator
    // Subnode A: first operand
    // Subnode B: second operand
    // ------------------------------------------
    while (peekTokenQueue(tokenList)->type == TOKEN_EQ || peekTokenQueue(tokenList)->type == TOKEN_NEQ) {
        TokenType oldType = peekTokenQueue(tokenList)->type;

        dequeueToken(tokenList);
        ParseResult right = parseRelational(tokenList, error);

        if (right.hasError || !right.node)
            return makeError(error, ERROR_UNDEFINED_SECOND_OPERAND, "Invalid right operand in equality comparison");

        SyntaxNode* n = initNode();
        n->type = NODE_BIN_OP;
        switch(oldType){
            case TOKEN_EQ:
                strcpy( n->operator, "==");
                break;
            case TOKEN_NEQ:
                strcpy( n->operator, "!=");
                break;
            default:
            ;
        }
        n->subnodeA = left.node;
        n->subnodeB = right.node;

        left.node = n;
    }

    return left;
}

ParseResult parseLogical(Token** tokenList, SimplicError* error) {
    ParseResult left = parseEquality(tokenList, error);
    if (left.hasError) return left;

    // ------------------------------------------
    // BINARY_OP Node -> operator
    // Subnode A: first operand
    // Subnode B: second operand
    // ------------------------------------------
    while (peekTokenQueue(tokenList)->type == TOKEN_AND || peekTokenQueue(tokenList)->type == TOKEN_OR) {
        TokenType oldType = peekTokenQueue(tokenList)->type;

        dequeueToken(tokenList);
        ParseResult right = parseEquality(tokenList, error);

        if (right.hasError || !right.node)
            return makeError(error, ERROR_UNDEFINED_SECOND_OPERAND, "Invalid right operand in logical comparison");

        SyntaxNode* n = initNode();
        n->type = NODE_BIN_OP;
        switch(oldType){
            case TOKEN_AND:
                strcpy( n->operator, "&&");
                break;
            case TOKEN_OR:
                strcpy( n->operator, "||");
                break;
            default:
            ;
        }
        n->subnodeA = left.node;
        n->subnodeB = right.node;

        left.node = n;
    }

    return left;
}

// Wrapper, used to parse the lowest precedence operation
ParseResult parseLowestPrecedenceOperation(Token** tokenList, SimplicError* error) {
    return parseLogical(tokenList, error);
}

SyntaxNode* parseTokenList(Token** tokenList, SimplicError* error) {
    ParseResult res = parseStatement(tokenList, error);
    return  res.node;
}

SyntaxNode* parseBlock(Token** tokenList, SimplicError* error, TokenType endToken) {
    // A block node has a list of ASTs (blockStatements) that will be run in one sitting by the interpreter
    // ------------------------------------------
    // BLOCK Node -> node list
    // ------------------------------------------
    
    SyntaxNode** blockStatements = NULL;
    int statementCount = 0;

    while (peekTokenQueue(tokenList)->type != endToken && !error->hasError && peekTokenQueue(tokenList)->type != TOKEN_EOF) {
        ParseResult statement = parseStatement(tokenList, error);

        // If null, we reached endToken
        if (statement.node == NULL) break;

        blockStatements = realloc(blockStatements, sizeof(SyntaxNode*) * (statementCount + 1));
        blockStatements[statementCount++] = statement.node;
    }

    if (peekTokenQueue(tokenList)->type != endToken && peekTokenQueue(tokenList)->type != TOKEN_EOF)
        makeError(error, ERROR_NON_TERMINATED_BLOCK, "Expected matching block terminator, instead received: %s", peekTokenQueue(tokenList)->name);

    if (peekTokenQueue(tokenList)->type == endToken)
        dequeueToken(tokenList); // consume endToken

    SyntaxNode* block = initNode();
    block->type = NODE_BLOCK;
    block->blockStatements = blockStatements;

    // One more space for the NULL delimiter
    block->blockStatements = realloc(blockStatements, sizeof(SyntaxNode*) * (statementCount + 1));
    block->blockStatements[statementCount] = NULL; // Null terminated statement list

    return block;
}
