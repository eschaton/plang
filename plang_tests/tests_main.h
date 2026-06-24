/*
    tests_support.h
    plangTests

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plangTests__tests_main__h__
#define __plangTests__tests_main__h__

#include <check.h>

#include "plang.h"


PLANG_HEADER_BEGIN


/*!
 Get the complete, configured test suite runner.

 Get the test suite covering all tests and a test runner that can be
 used to run it, without actually running any of the tests. In the
 future (with some API enhancements to Check) this will allow the test
 suite to be run under programmatic control for hooking into some other
 test infrastructure (such as Xcode's).
 */
SRunner * PLANG_NULLABLE
configured_tests_runner(void);


/*!
 Main entry point of tests.

 Run the full suite of tests as if from the command line.
 */
int
tests_main(int argc,
           char * PLANG_NULLABLE * PLANG_NONNULL argv);


PLANG_HEADER_END

#endif /* __plangTests__tests_main__h__ */
