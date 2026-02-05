BIN=main.exe
CC=gcc
CCFLAGS=-Wall -Wextra -pedantic -g

$(BIN): clean main.o input_handler.o commands_functions.o other_tools.o create_functions.o set_functions.o get_functions.o read_and_write_functions.o
	$(CC) main.o input_handler.o  commands_functions.o other_tools.o create_functions.o set_functions.o get_functions.o read_and_write_functions.o -o $(BIN) $(CCFLAGS)

main.o: main.c
	$(CC) -c main.c $(CCFLAGS)

input_handler.o: input_handler.c
	$(CC) -c input_handler.c $(CCFLAGS)

commands_functions.o: commands_functions.c
	$(CC) -c commands_functions.c $(CCFLAGS)

other_tools.o: other_tools.c
	$(CC) -c other_tools.c $(CCFLAGS)

create_functions.o: create_functions.c
	$(CC) -c create_functions.c $(CCFLAGS)

set_functions.o: set_functions.c
	$(CC) -c set_functions.c $(CCFLAGS)

get_functions.o: get_functions.c
	$(CC) -c get_functions.c $(CCFLAGS)

read_and_write_functions.o: read_and_write_functions.c
	$(CC) -c read_and_write_functions.c $(CCFLAGS)

clean:
	del *.o $(BIN) -f