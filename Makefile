default: release

CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -pedantic
DEBUGFLAGS = -g -O0 -fsanitize=address -fsanitize=leak
INCLUDES = -I include/ -I thirdparty/
BUILD_DIR = build

SRCS = src/lexer/lexer.c \
       src/parser/parser.c \
       src/interpreter/interpreter.c \
       src/simplicError/simplicError.c \
       src/main.c

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ----------- BUILD TARGETS -----------

release: $(BUILD_DIR)
	$(CC) $(SRCS) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/simplic

debug: $(BUILD_DIR)
	$(CC) $(SRCS) $(CFLAGS) $(DEBUGFLAGS) $(INCLUDES) -o $(BUILD_DIR)/simplic

# ----------- TEST BINARIES -----------

lexerTest: $(BUILD_DIR)
	$(CC) src/lexer/lexer.c src/lexer/lexer_test.c thirdparty/unity.c \
	$(CFLAGS) $(DEBUGFLAGS) $(INCLUDES) -o $(BUILD_DIR)/lexerTest

parserTest: $(BUILD_DIR)
	$(CC) src/lexer/lexer.c  src/simplicError/simplicError.c src/parser/parser.c src/parser/parser_test.c thirdparty/unity.c \
	$(CFLAGS) $(DEBUGFLAGS) $(INCLUDES) -o $(BUILD_DIR)/parserTest

interpreterTest: $(BUILD_DIR)
	$(CC) src/lexer/lexer.c src/simplicError/simplicError.c src/parser/parser.c src/interpreter/interpreter.c src/interpreter/interpreter_test.c thirdparty/unity.c \
	$(CFLAGS) $(DEBUGFLAGS) $(INCLUDES) -o $(BUILD_DIR)/interpreterTest

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
