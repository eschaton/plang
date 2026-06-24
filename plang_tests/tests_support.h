/*
    tests_support.h
    plangTests

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plangTests__tests_support__h__
#define __plangTests__tests_support__h__

#include <check.h>
#include <stdbool.h>

#include "plang.h"


PLANG_HEADER_BEGIN


/*! The test log; unique per setup invocation. */
extern plang_log_t PLANG_NULLABLE tests_log;


/*! Setup shared by all tests. */
void
tests_shared_setup(void);

/*! Teardown shared by all tests. */
void
tests_shared_teardown(void);


/*!
 Read a test file.

 Reads a test file from the `plang_tests` directory, returning its
 contents and size on success.

 - WARNING: The result buffer is allocated with `calloc(3)` and is the
            caller's responsibility to dealloate with `free(3)`.
 */
bool
tests_read_file(const char *name,
                char * PLANG_NULLABLE * PLANG_NONNULL data,
                size_t * PLANG_NONNULL data_len);


PLANG_HEADER_END

#endif /* __plangTests__tests_support__h__ */
