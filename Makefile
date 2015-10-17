#!/usr/bin/make -f

TARGET := librhid.so

C_FILES := $(wildcard src/*.c)
HEADERS := $(wildcard include/librhid/*.h)
PRIVATE_HEADERS := $(wildcard src/*.h)
VERSION := $(shell git rev-parse --short HEAD)
PREFIX ?= /usr/local

ifneq ($(shell which clang),)
	CC := clang
else
	CC := gcc
endif

CPPFLAGS += -Iinclude -DVERSION=$(VERSION)
CFLAGS += -std=gnu99 -Wall
OPTFLAGS += -Os

OBJS := ${C_FILES:%.c=%.c.o}

all: $(TARGET)

install: all
	mkdir -p $(PREFIX)/{lib,include/librhid}
	cp $(TARGET) $(PREFIX)/lib
	cp $(HEADERS) $(PREFIX)/include/librhid

$(TARGET): $(OBJS)
	$(CC) -shared $(LDFLAGS) -o $(TARGET) $(OBJS)

%.c.o: %.c $(HEADERS) $(PRIVATE_HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	-rm -f $(TARGET) $(OBJS)
