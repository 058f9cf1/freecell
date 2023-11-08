CC = gcc
CFLAGS = -Wall
SRCFILES = src/main.o src/cards.o src/display.o
TARGET = bin/freecell

all: $(TARGET)

$(TARGET): $(SRCFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCFILES) -lncurses

clean:
	rm -f $(SRCFILES)
