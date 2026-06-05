/*
    tests_support.c
    plangTests

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "tests_main.h"

#if PLANG_CLASCAL
#include "test_clascal_parsing.h"
#endif
#include "test_parsing.h"
#include "test_tokenization.h"


PLANG_SOURCE_BEGIN


SRunner * PLANG_NULLABLE
configured_tests_runner(void)
{
    SRunner *sr = srunner_create(NULL);

    srunner_add_suite(sr, tokenization_suite());
    srunner_add_suite(sr, parsing_suite());
#if PLANG_CLASCAL
    srunner_add_suite(sr, clascal_parsing_suite());
#endif

    return sr;
}


int
tests_main(int argc,
           char * PLANG_NULLABLE * PLANG_NONNULL argv)
{
    SRunner *sr = configured_tests_runner();

    srunner_run_all(sr, CK_VERBOSE);

    int number_failed = srunner_ntests_failed(sr);

    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


PLANG_SOURCE_END
