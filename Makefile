CC     = gcc
CFLAGS = -Wall -Wextra -std=c11

SRCS = Sand_string.c Sand_vector.c Sand_string_view.c atom.c mem.c
OBJS = $(SRCS:.c=.o)
LIB  = libsand.a

# Unity
UNITY_DIR  = tests/unity/src
UNITY_SRC  = $(UNITY_DIR)/unity.c
TESTCFLAGS = $(CFLAGS) -DUNITY_OUTPUT_COLOR -I$(UNITY_DIR)

# Test binaries
TEST_BINS = tests/test_sand_string tests/test_sand_string_view \
            tests/test_sand_vector tests/test_atom

all: $(LIB)

$(LIB): $(OBJS)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# --- Tests -------------------------------------------------------------------

tests/test_sand_string: tests/test_sand_string.c Sand_string.o $(UNITY_SRC)
	$(CC) $(TESTCFLAGS) -o $@ $^

tests/test_sand_string_view: tests/test_sand_string_view.c Sand_string_view.o $(UNITY_SRC)
	$(CC) $(TESTCFLAGS) -o $@ $^

tests/test_sand_vector: tests/test_sand_vector.c Sand_vector.o $(UNITY_SRC)
	$(CC) $(TESTCFLAGS) -DUNITY_INCLUDE_DOUBLE -o $@ $^

tests/test_atom: tests/test_atom.c atom.o $(UNITY_SRC)
	$(CC) $(TESTCFLAGS) -o $@ $^

test: $(TEST_BINS)
	@echo ""; \
	pass=0; fail=0; \
	for t in $(TEST_BINS); do \
		echo "\033[1;36m=== Running $$t ===\033[0m"; \
		./$$t; \
		if [ $$? -eq 0 ]; then pass=$$((pass+1)); else fail=$$((fail+1)); fi; \
		echo ""; \
	done; \
	echo "\033[1m========================================\033[0m"; \
	if [ $$fail -eq 0 ]; then \
		echo "\033[1;32m  All $$pass suites passed\033[0m"; \
	else \
		echo "\033[1;31m  Suites passed: $$pass   failed: $$fail\033[0m"; \
	fi; \
	echo "\033[1m========================================\033[0m"

clean:
	rm -f $(OBJS) $(LIB) $(TEST_BINS)

.PHONY: all clean test
