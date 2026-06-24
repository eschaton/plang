/*
    plangTestCase.m
    plang Tests

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#import "plangTestCase.h"

#include "tests_main.h"

#include <check.h>


NS_ASSUME_NONNULL_BEGIN


@implementation plangTestCase

- (void)testRunCheckTests
{
    // TODO: Generate real suite & test cases etc. for check tests
    // This requires API not available as of Check 0.15.2

    // Change the working directory to someplace the tests will be able
    // to find their test data.

    NSBundle *bundle = [NSBundle bundleForClass:self.class];
    NSFileManager *dfm = NSFileManager.defaultManager;
    BOOL changed = [dfm changeCurrentDirectoryPath:bundle.resourcePath];
    XCTAssertTrue(changed);

    // Get the test runner

    SRunner *sr = configured_tests_runner();

    // Turn off use of fork for the test suite run.

    srunner_set_fork_status(sr, CK_NOFORK);

    // Run all the tests in the test suite, with verbose output.

    srunner_run_all(sr, CK_VERBOSE);

    // Register any failures with Xcode, to get source annotations.

    int nfailed = srunner_ntests_failed(sr);

    TestResult **results = srunner_failures(sr);
    for (int i = 0; i < nfailed; i++) {
        TestResult *result = results[i];
        XCTIssue *issue = [self issueForCheckTestResult:result];
        [self recordIssue:issue];
    }

    // Clean up

    free(results);
    srunner_free(sr);
}

- (XCTIssue *)issueForCheckTestResult:(TestResult *)result
{
    enum test_result rtype = tr_rtype(result);
    int lno = tr_lno(result);
    const char *lfile = tr_lfile(result);
    const char *msg = tr_msg(result);

    XCTIssueType issueType = ((rtype == CK_FAILURE)
                              ? XCTIssueTypeAssertionFailure
                              : XCTIssueTypeUncaughtException);

    XCTSourceCodeLocation *sourceCodeLocation
        = [[XCTSourceCodeLocation alloc] initWithFilePath:@(lfile)
                                               lineNumber:lno];

    XCTSourceCodeContext *sourceCodeContext
        = [[XCTSourceCodeContext alloc] initWithLocation:sourceCodeLocation];

    return [[XCTIssue alloc] initWithType:issueType
                       compactDescription:@(msg)
                      detailedDescription:NULL
                        sourceCodeContext:sourceCodeContext
                          associatedError:NULL
                              attachments:@[]
                                 severity:XCTIssueSeverityError];
}

@end


NS_ASSUME_NONNULL_END
