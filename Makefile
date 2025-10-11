default: release

CC = gcc
INCLUDES = -I include/ -I thirdparty/

# Build mode, release or debug:
MODE ?= debug
RELEASEFLAGS = -std=c17 -Wall -Wextra -pedantic
DEBUGFLAGS = -g -O0 -fsanitize=address -fsanitize=leak

CFLAGS =
BUILD_DIR =

ifeq ($(MODE),debug)
CFLAGS = $(RELEASEFLAGS) $(DEBUGFLAGS)
BUILD_DIR = build/debug
else
CFLAGS = $(RELEASEFLAGS)
BUILD_DIR = build/release
endif

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ----------- BUILD TARGETS -----------

release: $(BUILD_DIR) lexer.o simplicError.o parser.o interpreter.o main.o
	$(CC) $(BUILD_DIR)/lexer.o $(BUILD_DIR)/simplicError.o $(BUILD_DIR)/parser.o $(BUILD_DIR)/interpreter.o $(BUILD_DIR)/main.o -o $(BUILD_DIR)/simplic

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

# ----------- TEST BINARIES -----------

lexerTest: $(BUILD_DIR)
	$(CC) src/lexer/lexer.c src/lexer/lexer_test.c thirdparty/unity.c \
	$(CFLAGS) $(DEBUGFLAGS) $(INCLUDES) -I src/lexer -o $(BUILD_DIR)/lexerTest

parserTest: $(BUILD_DIR)
	$(CC) src/lexer/lexer.c  src/simplicError/simplicError.c src/parser/parser.c src/parser/parser_test.c thirdparty/unity.c \
	$(CFLAGS) $(DEBUGFLAGS) $(INCLUDES) -I src/parser -o $(BUILD_DIR)/parserTest

interpreterTest: $(BUILD_DIR)
	$(CC) src/lexer/lexer.c src/simplicError/simplicError.c src/parser/parser.c src/interpreter/interpreter.c src/interpreter/interpreter_test.c thirdparty/unity.c \
	$(CFLAGS) $(DEBUGFLAGS) $(INCLUDES) -I src/interpreter -o $(BUILD_DIR)/interpreterTest

errorTest: $(BUILD_DIR)
	$(CC) src/simplicError/simplicError.c src/simplicError/simplicError_test.c thirdparty/unity.c \
	$(CFLAGS) $(DEBUGFLAGS) $(INCLUDES) -o $(BUILD_DIR)/errorTest

# ----------- TEST TARGETS -----------

# Solo construye los tests
test: lexerTest parserTest interpreterTest errorTest
	@echo "All tests built"

# Compila y los ejecuta uno por uno
runTest: test
	@echo "Running Tests..."
	@echo "-----------------------------"
	@./$(BUILD_DIR)/lexerTest || { echo "lexerTest failed"; exit 1; }
	@./$(BUILD_DIR)/parserTest || { echo "parserTest failed"; exit 1; }
	@./$(BUILD_DIR)/interpreterTest || { echo "interpreterTest failed"; exit 1; }
	@./$(BUILD_DIR)/errorTest || { echo "errorTest failed"; exit 1; }
	@echo "-----------------------------"
	@echo "All tests ran accordingly"

# ----------- CLEAN -----------

clean:
	rm -rf $(BUILD_DIR)
