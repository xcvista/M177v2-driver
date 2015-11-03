#!/usr/bin/make -f

TARGET := librhid.so

C_FILES := $(wildcard src/*.c)
HEADERS := $(wildcard include/librhid/*.h)
PRIVATE_HEADERS := $(wildcard src/*.h)
VERSION := $(shell git rev-parse --short HEAD)
PREFIX ?= /usr/local
TOOLS := gpio-export gpio-unexport gpio-read gpio-write gpio-mode gpio-wait

CC := $(if $(shell which clang),clang,gcc)
STRIP := strip

CPPFLAGS += -Iinclude -DVERSION=\"$(VERSION)\" $(if $(DEBUG),-DDEBUG,)
CFLAGS += -std=gnu99 -Wall -fPIC
OPTFLAGS += $(if $(DEBUG),-O0 -g,-Os)

OBJS := ${C_FILES:%.c=%.c.o}

all: $(TARGET) $(TOOLS)
.PHONY: all clean

install: all
	mkdir -p $(PREFIX)/{lib,bin,include/librhid}
	cp $(TARGET) $(PREFIX)/lib
	cp $(TOOLS) $(PREFIX)/bin
	cp $(HEADERS) $(PREFIX)/include/librhid

$(TARGET): $(OBJS)
	$(CC) -shared $(LDFLAGS) -o $(TARGET) $(OBJS)
	$(if $(DEBUG),,$(STRIP) -g $(TARGET))

%.c.o: %.c $(HEADERS) $(PRIVATE_HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OPTFLAGS) -c $< -o $@

%: tools/%.c.o tools/cli.c.o $(TARGET) $(HEADERS) tools/cli.h
	$(CC) $(LDFLAGS) -L. -lrhid tools/$@.c.o tools/cli.c.o -o $@
	$(if $(DEBUG),,$(STRIP) -s $@)

clean:
	-rm -f $(TARGET) $(OBJS) $(TOOLS) $(patsubst %,tools/%.c.o,$(TOOLS)) tools/cli.c.o
