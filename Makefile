TARGET = out/dmfmvc
CC = gcc
AR = ar
MAKE = make
CFLAGS = -Wall -Iinc -flto -O3 -L./lib -ldmf -std=c99
CHMOD = chmod

.PHONY: default all clean

default: $(TARGET)
all: default

HEADERS = $(wildcard *.h)
HEADERS += $(wildcard inc/*.h)

SRC_C =  $(wildcard *.c)
SRC_C += $(wildcard src/*.c)

OBJECTS = $(addprefix out/, $(SRC_C:.c=.o))

pre-build:
	mkdir -p out/
	mkdir -p out/src
	mkdir -p lib/

lib/libdmf.a:
	CC=$(CC) AR=$(AR) $(MAKE) -e -C dmf-parser/ all docs
	cp dmf-parser/out/libdmf.a lib/

out/%.o: %.c lib/libdmf.a $(HEADERS) pre-build
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): pre-build $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o $@
	$(CHMOD) +x $@

clean:
	-rm -f out/*.o
	-rm -f out/src/*.o
	-rm -f $(TARGET)
	-rm -f lib/*.a
	make -C dmf-parser/ clean
