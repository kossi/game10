CC=clang
CFLAGS=-Wall -Wextra -g -std=c11 -O2 -Isrc -DNDEBUG $(OPTFLAGS)
LIBS=-lGL -lglfw $(OPTLIBS)

CURPATH = $(shell pwd)
PROGRAM = $(shell basename $(CURPATH))
	
SOURCES = $(filter-out src/main.c, $(wildcard src/**/*.c src/*.c))
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

TARGET=build/lib$(PROGRAM).a
SO_TARGET=$(patsubst %.a,%.so,$(TARGET))

BIN_TARGET=$(PROGRAM)

all: $(TARGET) $(SO_TARGET) $(BIN_TARGET) tests

dev: CFLAGS=-g -Isrc -Wall -Wextra -std=c11 $(OPTFLAGS)
dev: all

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@

$(SO_TARGET): $(TARGET) $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS)

$(BIN_TARGET):
	$(CC) $(CFLAGS) $(LIBS) -fPIC $(SO_TARGET) -o $(PROGRAM) src/main.c

build: 
	@mkdir -p build

.PHONY: tests
tests:	CFLAGS += $(LIBS) -fPIC $(SO_TARGET)
tests: 	$(TESTS)
	sh ./tests/runtests.sh

valgrind:
	VALGRIND="valgrind --log-file=./tests/valgrind-%p.log" $(MAKE)

clean:
	rm -rf build $(OBJECTS) $(TESTS)
	rm -f tests/*.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`
	rm -f $(PROGRAM)
