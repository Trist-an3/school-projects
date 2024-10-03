CC = clang
CFLAGS = -Werror -Wall -Wextra -Wconversion -Wdouble-promotion -Wstrict-prototypes -pedantic -gdwarf-4
LFLAGS = -lm
TSP = tsp
HEADERS = graph.h path.h stack.h path.h vertices.h

all: $(TSP)

$(TSP): $(TSP).o graph.o path.o stack.o path.o
	$(CC) $^ $(LFLAGS) -o $@

$.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(TSP) *.o

format:
	clang-format -i -style=file *.[ch]

.PHONY: all clean format


