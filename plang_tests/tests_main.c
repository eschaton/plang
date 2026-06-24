/*
    tests_support.c
    plangTests

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "tests_main.h"

#include "test_parsing.h"
#include "test_tokenization.h"


PLANG_SOURCE_BEGIN


void
tests_suite_and_runner(Suite * PLANG_NULLABLE * PLANG_NONNULL s,
                       SRunner * PLANG_NULLABLE * PLANG_NONNULL sr)
{
    *s = suite_create("plang_tests");
    *sr = srunner_create(*s);

    srunner_add_suite(*sr, tokenization_suite());
    srunner_add_suite(*sr, parsing_suite());
}


int
tests_main(int argc,
           char * PLANG_NULLABLE * PLANG_NONNULL argv)
{
    Suite *s;
    SRunner *sr;
    tests_suite_and_runner(&s, &sr);

    srunner_run_all(sr, CK_VERBOSE);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


PLANG_SOURCE_END
