SOURCES := $(patsubst %.c,%.o,$(shell find . -name "*.c"))
HEADERS := $(shell find . -name "*.h")

CFLAGS  := -Wall -Wextra -Werror -pedantic -std=c99
CFLAGS	+= -pipe
CFLAGS	+= -fomit-frame-pointer -O3
CFLAGS	+= -g

all: calico $(SOURCES) $(HEADERS)

calico: $(SOURCES) $(HEADERS)
	@ echo " LD	" $(SOURCES)
	@ clang $(SOURCES) -o calico -lm

%.o: %.c $(HEADERS)
	@ echo " CC	" $<
	@ clang $(CFLAGS) -c $< -o $@

clean:
	@ rm $(SOURCES) calico
