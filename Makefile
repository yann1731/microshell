SRCS =		main.c

CFLAGS =	-Wall -Werror -Wextra

OBJS = ${SRCS:.c=.o}

NAME = microshell

GCC = gcc

${NAME}: ${OBJS}
	${GCC} ${CFLAGS} ${OBJS} -o ${NAME}

all: ${NAME}

clean:
	rm -f ${OBJS}

fclean: clean
	rm -f ${NAME}