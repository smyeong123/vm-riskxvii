TARGET = vm_riskxvii

CC = gcc

# CFLAGS     = -c -Wall -Wvla -Werror -O0 -g -std=c11
CFLAGS     = -c -lm -s -fno-exceptions -Wall -Wvla -Werror -O0 -std=c11
ASAN_FLAGS = -lm -fsanitize=address
SRC        = vm_riskxvii.c
OBJ        = $(SRC:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) $(ASAN_FLAGS) -o $@ $(OBJ)

.SUFFIXES: .c .o

.c.o:
	 $(CC) $(CFLAGS) $(ASAN_FLAGS) $<

run:
	./$(TARGET)

test:
	echo what are we testing?!

clean:
	rm -f *.o *.obj $(TARGET)
