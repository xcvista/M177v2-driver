#!/usr/bin/make -f

TARGET := librhid.so

C_FILES := $(wildcard src/*.c)
HEADERS := $(wildcard include/librhid/*.h)
PRIVATE_HEADERS := $(wildcard src/*.h)
VERSION := $(shell git rev-parse --short HEAD)
PREFIX ?= /usr/local

TOOLS := gpio-export gpio-unexport gpio-read gpio-write gpio-mode gpio-wait
TOOLS_C_FILES := $(wildcard tools/*.c)
TOOLS_OBJS := ${TOOLS_C_FILES:%.c=%.c.o}

DEMO := rhid-demo
DEMO_C_FILES := $(wildcard demo/*.c)
DEMO_OBJS := ${DEMO_C_FILES:%.c=%.c.o}

CC := $(if $(shell which clang),clang,gcc)
STRIP := strip

CPPFLAGS += -Iinclude -DVERSION=\"$(VERSION)\" $(if $(DEBUG),-DDEBUG,)
CFLAGS += -std=gnu99 -Wall -fPIC
OPTFLAGS += $(if $(DEBUG),-O0 -g,-Os)
LIBS += -lpthread

OBJS := ${C_FILES:%.c=%.c.o}

all: $(TARGET)
.PHONY: all clean tools demo

tools: $(TOOLS)

demo: $(DEMO)
	sudo env LD_LIBRARY_PATH=. $(if $(DEBUG),gdb,) ./$(DEMO)

install: all
	mkdir -p $(PREFIX)/{lib,include/librhid}
	cp $(TARGET) $(PREFIX)/lib
#	cp $(TOOLS) $(PREFIX)/bin
	cp $(HEADERS) $(PREFIX)/include/librhid

$(TARGET): $(OBJS)
	$(CC) -shared $(LDFLAGS) $(LIBS) -o $(TARGET) $(OBJS)
	$(if $(DEBUG),,$(STRIP) -g $(TARGET))

%.c.o: %.c $(HEADERS) $(PRIVATE_HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OPTFLAGS) -c $< -o $@

%: tools/%.c.o tools/cli.c.o $(TARGET) $(HEADERS) tools/cli.h
	$(CC) $(LDFLAGS) -L. -lrhid tools/$@.c.o tools/cli.c.o -o $@
	$(if $(DEBUG),,$(STRIP) -s $@)

$(DEMO): $(DEMO_OBJS) $(TARGET) $(HEADERS)
	$(CC) $(LDFLAGS) -L. -lrhid $(DEMO_OBJS) -o $@
	$(if $(DEBUG),,$(STRIP) -s $@)

clean:
	-rm -f $(TARGET) $(OBJS) $(TOOLS) $(TOOLS_OBJS) $(DEMO) $(DEMO_OBJS)
