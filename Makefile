default: simplic

CC = gcc
INCLUDES = -I include/ -I thirdparty/
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

simplic: $(BUILD_DIR) lexer.o simplicError.o parser.o interpreter.o main.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/lexer.o $(BUILD_DIR)/simplicError.o $(BUILD_DIR)/parser.o $(BUILD_DIR)/interpreter.o $(BUILD_DIR)/main.o -o $(BUILD_DIR)/simplic

# ----------- BUILD OBJECTS -----------

lexer.o: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -I src/lexer -c src/lexer/lexer.c -o $(BUILD_DIR)/lexer.o

simplicError.o: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c src/simplicError/simplicError.c -o $(BUILD_DIR)/simplicError.o

parser.o: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -I src/parser -c src/parser/parser.c -o $(BUILD_DIR)/parser.o

interpreter.o: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -I src/interpreter -c src/interpreter/interpreter.c -o $(BUILD_DIR)/interpreter.o

main.o: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c src/main.c -o $(BUILD_DIR)/main.o

# Test framework
unity.o: $(TEST_DIR)
	$(CC) $(CFLAGS) -I thirdparty/ -c thirdparty/unity.c -o $(TEST_DIR)/unity.o


# ----------- TEST BINARIES -----------

lexerTest: $(TEST_DIR) unity.o simplicError.o
	$(CC) $(TESTADITIONALFLAGS) $(CFLAGS) $(INCLUDES) -I src/lexer/ src/lexer/lexer_test.c $(TEST_DIR)/unity.o $(BUILD_DIR)/simplicError.o -o $(TEST_DIR)/lexerTest

parserTest: $(TEST_DIR) unity.o simplicError.o lexer.o
	$(CC) $(TESTADITIONALFLAGS) $(CFLAGS) $(INCLUDES) -I src/parser/ src/parser/parser_test.c $(BUILD_DIR)/lexer.o $(TEST_DIR)/unity.o $(BUILD_DIR)/simplicError.o -o $(TEST_DIR)/parserTest

interpreterTest: $(TEST_DIR) unity.o simplicError.o lexer.o parser.o
	$(CC) $(TESTADITIONALFLAGS) $(CFLAGS) $(INCLUDES)  -I src/interpreter/ src/interpreter/interpreter_test.c $(BUILD_DIR)/lexer.o $(TEST_DIR)/unity.o $(BUILD_DIR)/simplicError.o $(BUILD_DIR)/parser.o -o $(TEST_DIR)/interpreterTest

errorTest: $(TEST_DIR) unity.o
	$(CC) $(TESTADITIONALFLAGS) $(CFLAGS) $(INCLUDES) -I src/simplicError/ src/simplicError/simplicError_test.c $(BUILD_DIR)/lexer.o $(TEST_DIR)/unity.o -o $(TEST_DIR)/errorTest

# ----------- TEST TARGETS -----------

test: lexerTest parserTest interpreterTest errorTest
	@echo "All tests built"

runTest: test
	@echo "Running Tests..."
	@echo "-----------------------------"
	@./$(TEST_DIR)/lexerTest || { echo "lexerTest failed"; exit 1; }
	@./$(TEST_DIR)/parserTest || { echo "parserTest failed"; exit 1; }
	@./$(TEST_DIR)/interpreterTest || { echo "interpreterTest failed"; exit 1; }
	@./$(TEST_DIR)/errorTest || { echo "errorTest failed"; exit 1; }
	@echo "-----------------------------"
	@echo "All tests ran accordingly"

# ----------- CLEAN -----------

clean:
	rm -rf $(BUILD_ROOT_DIR)
