CC = clang
CFLAGS = -Werror -Wall -Wextra -Wconversion -Wdouble-promotion -Wstrict-prototypes -pedantic -gdwarf-4
LFLAGS = -lm
COLORB = colorb
IOTEST = iotest
HEADERS = bmp.h io.h 

all: $(COLORB) $(IOTEST)

$(COLORB): $(COLORB).o bmp.o io.o
	$(CC) $^ $(LFLAGS) -o $@

$(IOTEST): $(IOTEST).o bmp.o io.o
	$(CC) $^ $(LFLAGS) -o $@

$.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(COLORB) $(IOTEST) *.o

format:
	clang-format -i -style=file *.[ch]

.PHONY: all clean format
