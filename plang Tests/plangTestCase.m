/*
    plangTestCase.m
    plang Tests

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#import "plangTestCase.h"


NS_ASSUME_NONNULL_BEGIN


@implementation plangTestCase
{
    plang_log_t _log;
    NSMutableArray *_messages;
}

- (void)logMessage:(NSString *)message
           atLevel:(plang_log_level_t)level
  indentationLevel:(int)indent
{
    NSDictionary *typeMap = @{
        @(plang_log_level_debug):   @"debug",
        @(plang_log_level_info):    @"info",
        @(plang_log_level_notice):  @"notice",
        @(plang_log_level_warning): @"warning",
        @(plang_log_level_error):   @"error",
    };

    NSString *fullMessage = [NSString stringWithFormat:@"%@: %@",
                             typeMap[@(level)], message];
    [_messages addObject:fullMessage];
}

void plangTestCasLogOutput(const plang_log_level_t level,
                           const char *message,
                           int indent,
                           void * PLANG_NONNULL context)
{
    plangTestCase *testCase = (__bridge plangTestCase *) context;

    [testCase logMessage:[NSString stringWithUTF8String:message]
                 atLevel:level
        indentationLevel:indent];
}

- (void)setUp
{
    [super setUp];

    _log = plang_log_new(plangTestCasLogOutput, (__bridge void *) self);
    XCTAssertNotEqual(_log, NULL);

    /* Log everything */
    plang_log_set_default_level(_log, plang_log_level_debug);

    _messages = [NSMutableArray array];
}

- (void)tearDown
{
    plang_log_free(_log);

    [super tearDown];
}

- (plang_log_t)plang_log
{
    return _log;
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
