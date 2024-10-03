CC = clang
CFLAGS = -Werror -Wall -Wextra -Wconversion -Wdouble-promotion -Wstrict-prototypes -pedantic -gdwarf-4
LFLAGS = -lm
HUFF = huff
DEHUFF = dehuff
BRTEST = brtest
BWTEST = bwtest
NODETEST = nodetest
PQTEST = pqtest
HEADERS = bitreader.h bitwriter.h node.h pq.h
all: $(HUFF) $(DEHUFF) $(BRTEST) $(BWTEST) $(NODETEST) $(PQTEST)


$(HUFF): $(HUFF).o bitreader.o bitwriter.o node.o pq.o
	$(CC) $^ $(LFLAGS) -o  $@

$(DEHUFF): $(DEHUFF).o bitreader.o bitwriter.o node.o pq.o
	$(CC) $^ $(LFLAGS) -o  $@

$(BRTEST): $(BRTEST).o bitreader.o
	$(CC) $^ $(LFLAGS) -o  $@

$(BWTEST): $(BWTEST).o bitwriter.o
	$(CC) $^ $(LFLAGS) -o  $@

$(NODETEST): $(NODETEST).o node.o
	$(CC) $^ $(LFLAGS) -o  $@

$(PQTEST): $(PQTEST).o pq.o node.o
	$(CC) $^ $(LFLAGS) -o  $@

$.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(HUFF) $(DEHUFF) $(BRTEST) $(BWTEST) $(NODETEST) $(PQTEST) *.o

format:
	clang-format -i -style=file *.[ch]

.PHONY: all clean format
