/*
    plangTestCase.m
    plang Tests

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#import "plangTestCase.h"


NS_ASSUME_NONNULL_BEGIN


@implementation plangTestCase

- (void)setUp
{
    [super setUp];

    // TODO: Other set-up
}

- (void)tearDown
{
    // TODO: Other tear-down
    
    [super tearDown];
}

- (nullable NSData *)testFileWithName:(NSString *)name
                                error:(NSError **)error
{
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *testFilePath = [bundle pathForResource:name ofType:@"pas"];
    XCTAssertNotNil(testFilePath);
    return [NSData dataWithContentsOfFile:testFilePath options:0 error:error];
}

@end


NS_ASSUME_NONNULL_END
