TARGET = pvect
CC = gcc
CFLAGS = -g -Wall -pthread
OBJS = pvect.o
SOURCES = $(addsuffix .c, $(basename $(OBJS)))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c  $< -o $@

clean:
	-rm -f *.o $(TARGET)
