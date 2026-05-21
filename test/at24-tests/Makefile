# Processor Companion Utility (PCU) Test Makefile
# Copyright (c) 2024, Texas Instruments Incorporated
# This is for the at24-tester
# Intended to be build on Sitara target running Linux

PRJ_NAME = at24-tester
SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c,%.o, $(SOURCES))
OUT_FILE = $(PRJ_NAME).out

CC = gcc

$(OUT_FILE) : $(OBJECTS)
	$(CC) -o $(OUT_FILE) $(OBJECTS)

$(OBJECTS) : $(SOURCES)
	$(CC) -c $(SOURCES)

.PHONY clean :
	rm $(OBJECTS) $(OUT_FILE)
