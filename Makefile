SOURCES := $(patsubst %.cpp,%.o,$(shell find . -name "*.cpp"))
HEADERS := $(shell find . -name "*.h")

CFLAGS  := -Wall -Wextra -Werror -pedantic -std=c++0x
CFLAGS	+= -pipe
CFLAGS	+= -fomit-frame-pointer -O3

all: calico $(SOURCES) $(HEADERS)

calico: $(SOURCES) $(HEADERS)
	@ echo " LD	" $(SOURCES)
	@ clang $(SOURCES) -o calico -lm

%.o: %.cpp $(HEADERS)
	@ echo " CPP	" $<
	@ clang $(CFLAGS) -c $< -o $@

clean:
	@ rm $(SOURCES) calico
