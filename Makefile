CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -std=c17 -fsanitize=address,undefined

OBJS = runner.o logic.o hash_table.o

all: runner

runner: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o runner

prog_assemble:
	as prog.s -o tmp.o && objcopy tmp.o -O binary prog.bin && rm tmp.o && make && qemu runner < prog.bin
