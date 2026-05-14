/*
    plangTestCase.h
    plang Tests

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#import <XCTest/XCTest.h>

#import "plang.h"


NS_ASSUME_NONNULL_BEGIN


@interface plangTestCase : XCTestCase

/*!
 Gets a test file.

 Returns the contents of the test file with the given name in the test
 bundle's resources.
 */
- (nullable NSData *)testFileWithName:(NSString *)name
                                error:(NSError **)error;

@end


NS_ASSUME_NONNULL_END
