SRC	= callbacks.c
OBJ	= $(SRC:.c=.o)

CC	= x86_64-w64-mingw32-gcc
CFLAGS	= -Wall -Wextra -mtune=native -march=native

all:	$(OBJ)
	$(CC) $(OBJ) $(CFLAGS)

$(OBJ):	$(SRC)

.PHONY:	clean

clean:
	rm $(OBJ) *.exe
