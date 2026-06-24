#
#    plang.h
#    plang
#
#    Copyright © 2026 Christopher M. Hanson. All rights reserved.
#    See the file COPYING for license details.
#

OBJDIR = obj
SRCDIR = plang
TESTS_DIR = plang_tests

PLANG_OBJS = \
	$(OBJDIR)/plang.o \
	$(OBJDIR)/plang_array.o \
	$(OBJDIR)/plang_constant.o \
	$(OBJDIR)/plang_dictionary.o \
	$(OBJDIR)/plang_error.o \
	$(OBJDIR)/plang_function.o \
	$(OBJDIR)/plang_log.o \
	$(OBJDIR)/plang_node.o \
	$(OBJDIR)/plang_parser.o \
	$(OBJDIR)/plang_procedure.o \
	$(OBJDIR)/plang_program.o \
	$(OBJDIR)/plang_range.o \
	$(OBJDIR)/plang_scope.o \
	$(OBJDIR)/plang_source.o \
	$(OBJDIR)/plang_token.o \
	$(OBJDIR)/plang_type.o \
	$(OBJDIR)/plang_unit.o \
	$(OBJDIR)/plang_variable.o

DEMO_OBJS = \
	$(OBJDIR)/plang_demo.o

TEST_OBJS = \
	$(OBJDIR)/test_parsing.o \
	$(OBJDIR)/test_tokenization.o \
	$(OBJDIR)/tests_main.o \
	$(OBJDIR)/tests_support.o

CFLAGS += -Os -I plang -Wall

PKG_CONFIG = $(shell which pkg-config)
CHECK_CFLAGS = $(shell $(PKG_CONFIG) --cflags check) -Dtests_main=main
CHECK_LDFLAGS = $(shell $(PKG_CONFIG) --libs check)


### Build Rules

.SUFFIXES: .c .o

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: plang_demo/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(TESTS_DIR)/%.c
	$(CC) $(CFLAGS) $(CHECK_CFLAGS) -c -o $@ $<


### Primary Targets

all: plang_demo_tool plang_test_tool

plang_demo_tool: Makefile $(OBJDIR) $(PLANG_OBJS) $(DEMO_OBJS)
	$(CC) $(PLANG_OBJS) $(DEMO_OBJS) -o $@

plang_test_tool: Makefile $(OBJDIR) $(PLANG_OBJS) $(TEST_OBJS)
	$(CC) -o $@ $(CHECK_LDFLAGS) $(PLANG_OBJS) $(TEST_OBJS)

check: plang_test_tool
	./plang_test_tool

clean:
	$(RM) $(PLANG_OBJS)
	$(RM) $(DEMO_OBJS)
	$(RM) plang_demo_tool
	$(RM) $(TEST_OBJS)
	$(RM) plang_test_tool
	$(RM) -r $(OBJDIR)


### Utility Targets

.PHONY: $(OBJDIR)
$(OBJDIR):
	@mkdir -p $(OBJDIR)
