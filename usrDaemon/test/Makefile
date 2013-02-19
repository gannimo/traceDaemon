include ../Makedefs

# use g++ compiler
CC=g++

TRACEDAEMON=$(LIBNAME).so.$(LIBVERS).$(LIBMIN)

INCLUDEDIR=../

# Points to the root of Google Test, relative to where this file is.
# Remember to tweak this if you move this file.
GTEST_DIR = /usr/src/gtest

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = /usr/include/gtest/*.h \
                /usr/include/gtest/internal/*.h

# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

SOURCES = $(wildcard *.cc)
OBJECTS = $(SOURCES:.cc=.o)

.PHONY: runtest build clean

all: runtest

build: gtest_main.a $(OBJECTS)
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -I$(GTEST_DIR)/include -lpthread \
		*.o ../*.o gtest_main.a -o test

%.o: %.cc
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -I$(GTEST_DIR)/include -c $< -o $@

runtest: build
	./test

clean:
	rm -f *.o test *.as *.out gtest_main.c *~

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
gtest-all.o : $(GTEST_SRCS_)
	$(CC) -I$(GTEST_DIR)/include -I$(GTEST_DIR) $(CFLAGS) -c \
		$(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CC) -I$(GTEST_DIR)/include -I$(GTEST_DIR) $(CFLAGS) -c \
		$(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^
