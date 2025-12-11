default: simplic

CC = gcc
BIN_NAME = "simplic"
INCLUDES = -I include/ -I include/dataStructures -I thirdparty/
BUILD_ROOT_DIR = build

TEST_DIR = $(BUILD_ROOT_DIR)/tests

# Build mode, release or debug:
MODE ?= release
RELEASEFLAGS = -std=c17 -Wall -Wextra -pedantic -fvisibility=hidden
DEBUGFLAGS = -g -O0 -fsanitize=address -fsanitize=leak
TESTADITIONALFLAGS = -Wno-unused-result -Wno-unused-function -DUNIT_TEST

CFLAGS =
BUILD_DIR =

ifeq ($(MODE),debug)
CFLAGS = $(RELEASEFLAGS) $(DEBUGFLAGS)
BUILD_DIR = $(BUILD_ROOT_DIR)/debug
else
CFLAGS = $(RELEASEFLAGS)
BUILD_DIR = $(BUILD_ROOT_DIR)/release
endif

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TEST_DIR):
	mkdir -p $(TEST_DIR)

# ----------- BUILD TARGETS -----------

simplic: $(BUILD_DIR) token.o lexer.o simplicError.o parser.o memoryBank.o interpreter.o scriptReader.o ast.o main.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/token.o $(BUILD_DIR)/lexer.o $(BUILD_DIR)/simplicError.o $(BUILD_DIR)/parser.o $(BUILD_DIR)/memoryBank.o $(BUILD_DIR)/interpreter.o $(BUILD_DIR)/scriptReader.o $(BUILD_DIR)/ast.o $(BUILD_DIR)/main.o -o $(BUILD_DIR)/$(BIN_NAME)

run: simplic
	./$(BUILD_DIR)/$(BIN_NAME)

# ----------- BUILD OBJECTS -----------

ast.o: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -I src/dataStructures/ast -c src/dataStructures/AST/ast.c -o $(BUILD_DIR)/ast.o

token.o: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -I src/dataStructures/token -c src/dataStructures/token/token.c -o $(BUILD_DIR)/token.o

memoryBank.o: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -I src/dataStructures/memoryBank -c src/dataStructures/memoryBank/memoryBank.c -o $(BUILD_DIR)/memoryBank.o

lexer.o: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -I src/lexer -c src/lexer/lexer.c -o $(BUILD_DIR)/lexer.o

simplicError.o: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c src/simplicError/simplicError.c -o $(BUILD_DIR)/simplicError.o

parser.o: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -I src/parser -c src/parser/parser.c -o $(BUILD_DIR)/parser.o

interpreter.o: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -I src/interpreter -c src/interpreter/interpreter.c -o $(BUILD_DIR)/interpreter.o

scriptReader.o: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c src/scriptReader/scriptReader.c -o $(BUILD_DIR)/scriptReader.o

main.o: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c src/main.c -o $(BUILD_DIR)/main.o

# Test framework
unity.o: $(TEST_DIR)
	$(CC) $(CFLAGS) -I thirdparty/ -c thirdparty/unity.c -o $(TEST_DIR)/unity.o


# ----------- TEST BINARIES -----------

tokenTest: $(TEST_DIR) unity.o simplicError.o
	$(CC) $(TESTADITIONALFLAGS) $(CFLAGS) $(INCLUDES) -I src/dataStructures/token/ src/dataStructures/token/token_test.c $(TEST_DIR)/unity.o $(BUILD_DIR)/simplicError.o -o $(TEST_DIR)/tokenTest

lexerTest: $(TEST_DIR) unity.o simplicError.o token.o
	$(CC) $(TESTADITIONALFLAGS) $(CFLAGS) $(INCLUDES) -I src/lexer/ src/lexer/lexer_test.c $(TEST_DIR)/unity.o $(BUILD_DIR)/simplicError.o $(BUILD_DIR)/token.o -o $(TEST_DIR)/lexerTest

parserTest: $(TEST_DIR) unity.o simplicError.o token.o lexer.o ast.o
	$(CC) $(TESTADITIONALFLAGS) $(CFLAGS) $(INCLUDES) -I src/parser/ src/parser/parser_test.c  $(BUILD_DIR)/token.o $(BUILD_DIR)/lexer.o $(BUILD_DIR)/ast.o $(TEST_DIR)/unity.o $(BUILD_DIR)/simplicError.o -o $(TEST_DIR)/parserTest

interpreterTest: $(TEST_DIR) unity.o simplicError.o token.o lexer.o parser.o ast.o memoryBank.o
	$(CC) $(TESTADITIONALFLAGS) $(CFLAGS) $(INCLUDES)  -I src/interpreter/ src/interpreter/interpreter_test.c  $(BUILD_DIR)/token.o $(BUILD_DIR)/lexer.o $(BUILD_DIR)/ast.o $(TEST_DIR)/unity.o $(BUILD_DIR)/simplicError.o $(BUILD_DIR)/parser.o $(BUILD_DIR)/memoryBank.o -o $(TEST_DIR)/interpreterTest

errorTest: $(TEST_DIR) unity.o
	$(CC) $(TESTADITIONALFLAGS) $(CFLAGS) $(INCLUDES) -I src/simplicError/ src/simplicError/simplicError_test.c  $(TEST_DIR)/unity.o -o $(TEST_DIR)/errorTest

# ----------- TEST TARGETS -----------

test: tokenTest lexerTest parserTest interpreterTest errorTest
	@echo "All tests built"

runTest: test
	@echo "Running Tests..."
	@echo "-----------------------------"
	@./$(TEST_DIR)/tokenTest || { echo "tokenTest failed"; exit 1; }
	@./$(TEST_DIR)/lexerTest || { echo "lexerTest failed"; exit 1; }
	@./$(TEST_DIR)/parserTest || { echo "parserTest failed"; exit 1; }
	@./$(TEST_DIR)/interpreterTest || { echo "interpreterTest failed"; exit 1; }
	@./$(TEST_DIR)/errorTest || { echo "errorTest failed"; exit 1; }
	@echo "-----------------------------"
	@echo "All tests ran accordingly"

# ----------- CLEAN -----------

clean:
	rm -rf $(BUILD_ROOT_DIR)
