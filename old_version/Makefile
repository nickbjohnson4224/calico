SOURCES := $(patsubst %.c,%.o,$(shell find . -mindepth 2 -name "*.c"))
HEADERS := $(shell find . -name "*.h")

CFLAGS  := -Wall -Wextra -Werror -pedantic -std=gnu99 -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable
CFLAGS	+= -pipe
CFLAGS	+= -fomit-frame-pointer -O3 -march=native
CFLAGS	+= -g
CFLAGS	+= -I$(PWD)/libcalico/inc

all: calico-learn calico libcalico.a $(SOURCES) $(HEADERS)

calico-learn: libcalico.a learn.o
	@ echo " LD	" libcalico.a learn.o
	@ gcc $(CFLAGS) -o calico-learn learn.o libcalico.a -lm

calico: libcalico.a main.o
	@ echo " LD	" libcalico.a main.o
	@ gcc $(CFLAGS) -o calico main.o libcalico.a -lm -lSDL

libcalico.a: $(SOURCES) $(HEADERS)
	@ echo " AR	" $(SOURCES)
	@ ar rcs libcalico.a $(SOURCES)

%.o: %.c $(HEADERS)
	@ echo " CC	" $<
	@ gcc $(CFLAGS) -c $< -o $@

clean:
	@ rm $(SOURCES) calico
