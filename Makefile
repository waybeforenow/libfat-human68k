DEBUG=false
CFLAGS=-Wall 
CC=gcc
AR=ar

ifeq ($(DEBUG),true)
	CFLAGS+=-D_DEBUG -g -Og 
else
	CFLAGS+= -O2
endif

ifeq ($(OS),Windows_NT)
	CFLAGS+=-D_INC_TCHAR
endif

all: libfat-human68k.a

libfat-human68k.a: diskio.o ff.o option/cc932.o 
	$(AR) cvq $@ $^

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f libfat-human68k.a *.o option/*.o
