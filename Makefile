TARGET = out/dmfmvc
LIBS = -lm
CC = gcc
CFLAGS = -Wall -Iinc -flto -O3 -g
CHMOD = chmod

.PHONY: default all clean

default: $(TARGET)
all: default
run: all

HEADERS = $(wildcard *.h)
HEADERS += $(wildcard inc/*.h)

SRC_C =  $(wildcard *.c)
SRC_C += $(wildcard src/*.c)

OBJECTS = $(addprefix out/, $(SRC_C:.c=.o))

out/%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $(LIBS) -o $@
	$(CHMOD) +x $@

clean:
	-rm -f out/*.o
	-rm -f out/src/*.o
	-rm -f $(TARGET)

run: $(TARGET)
	cd out/ && gnome-terminal -- "../$(TARGET)"
