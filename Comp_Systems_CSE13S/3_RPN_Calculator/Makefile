CC = clang
CFLAGS = -Werror -Wall -Wextra -Wconversion -Wdouble-promotion -Wstrict-prototypes -pedantic 
LFLAGS = -lm
CALC = calc
TEST = tests
HEADERS = mathlib.h messages.h operators.h stack.h

all: $(CALC) $(TEST)

$(CALC): $(CALC).o mathlib.o operators.o stack.o
	$(CC) $^ $(LFLAGS) -o $@

$(TEST): $(TEST).o mathlib.o operators.o stack.o
	$(CC) $^ $(LFLAGS) -o $@

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(CALC) $(TEST) *.o

format:
	clang-format -i -style=file *.[ch]

.PHONY: all clean format
