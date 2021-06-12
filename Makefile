CC = gcc
FLAGS = -Wall -Wextra 
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
HEADERS = $(wildcard includes/*.h)

all: run

%.o: %.c
	${CC} ${FLAGS} -c $^ -o $@

hive: ${SRCS}
	${CC} $^ -o $@

run: hive
	./hive

clean:
	rm *.o hive
