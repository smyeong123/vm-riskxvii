TARGET = vm_riskxvii
CC = gcc

CFLAGS     = -lm -s -fno-exceptions -Wall -Wvla -Werror -O0 -std=c11
ASAN_FLAGS = -lm -fsanitize=address
SRC        = vm_riskxvii.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(ASAN_FLAGS) -o $(TARGET) $(SRC)

run:
	./$(TARGET)

test:
	echo what are we testing?!

clean:
	rm -f $(TARGET)
