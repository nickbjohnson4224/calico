SOURCES := $(patsubst %.c,%.o,$(shell find src -name "*.c"))
HEADERS := $(shell find src -name "*.h")

CFLAGS  := -Werror -pedantic -std=c99
CFLAGS	+= -pipe
CFLAGS	+= -fomit-frame-pointer -O3

all: calico $(SOURCES) $(HEADERS)

calico: $(SOURCES) $(HEADERS)
	@ echo " LD	" $(SOURCES)
	@ clang $(SOURCES) -o calico -lm

%.o: %.c
	@ echo " CC	" $<
	@ clang $(CFLAGS) -c $< -o $@

clean:
	@ rm $(SOURCES) src/calico.o
