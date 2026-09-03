CC = gcc
CFLAGS = $(shell pkg-config --cflags gtk+-3.0 vte-2.91) -Os -s
LIBS = $(shell pkg-config --libs gtk+-3.0 vte-2.91)

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

TARGET = jetty
SRC = jetty.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LIBS)

install: $(TARGET)
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)

clean:
	rm -f $(TARGET)
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)

.PHONY: all install clean
