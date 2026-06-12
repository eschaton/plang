#
#    plang.h
#    plang
#
#    Copyright © 2026 Christopher M. Hanson. All rights reserved.
#    See the file COPYING for license details.
#

OBJS = \
	plang/plang.o \
	plang/plang_array.o \
	plang/plang_constant.o \
	plang/plang_dictionary.o \
	plang/plang_error.o \
	plang/plang_function.o \
	plang/plang_node.o \
	plang/plang_parser.o \
	plang/plang_procedure.o \
	plang/plang_program.o \
	plang/plang_range.o \
	plang/plang_scope.o \
	plang/plang_source.o \
	plang/plang_token.o \
	plang/plang_type.o \
	plang/plang_unit.o \
	plang/plang_variable.o

CFLAGS += -Os -Wall -DPLANG_MAIN

.o: .c
	$(CC) $(CFLAGS) -c $< -o $@

plang_driver: Makefile $(OBJS)
	$(CC) $(OBJS) -o $@

all: plang_driver

clean:
	$(RM) plang_driver $(OBJS)
