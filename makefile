CC = gcc
CFLAGS = -Wall
SRCFILES = src/main.o src/generate_deal.o src/show.o
TARGET = bin/freecell

all: $(TARGET)

$(TARGET): $(SRCFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCFILES) -lncurses

clean:
	rm -f $(SRCFILES)
