/*
    plangTestParsing.m
    plang Tests

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#import "plangTestCase.h"

#include "plang_parser_internal.h"

#include "plang_node_internal.h"
#include "plang_procedure.h"
#include "plang_scope.h"
#include "plang_token.h"


NS_ASSUME_NONNULL_BEGIN


@interface plangTestParsing : plangTestCase
@end


@implementation plangTestParsing

- (void)testParseTrivialUnit
{
    NSError *error = nil;
    NSData *file = [self testFileWithName:@"TrivialUnit" error:&error];
    XCTAssertNotNil(file, @"Couldn't get file: %@", error);
    const char *buf = [file bytes];
    const size_t buf_len = [file length];

    plang_source_t source = plang_source_new(NULL, buf, buf_len);
    XCTAssertNotEqual(source, NULL);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    XCTAssertNotEqual(sources, NULL);
    plang_parser_t parser = plang_parser_new(sources, self.plang_log);
    XCTAssertNotEqual(parser, NULL);

    bool parsed = plang_parser_run(parser);
    plang_array_t errors = plang_parser_copy_errors(parser);
    const size_t errors_count = errors ? plang_array_get_count(errors) : 0;
    XCTAssertTrue(parsed, @"Parse failed, %zu errors", errors_count);

    for (size_t error_idx = 0; error_idx < errors_count; error_idx++) {
        plang_error_t error = plang_array_get_item(errors, error_idx);
        XCTAssertNotEqual(error, NULL);
        const char *error_text = plang_error_copy_text(error);
        XCTAssertNotEqual(error_text, NULL);
        XCTAssertTrue(false, "%zu: %s", error_idx, error_text);
        free((void *)error_text);
    }

    plang_parser_free(parser);
    plang_source_free(source);
}

- (void)testParsePreambleUnit
{
    NSError *error = nil;
    NSData *file = [self testFileWithName:@"PreambleUnit" error:&error];
    XCTAssertNotNil(file, @"Couldn't get file: %@", error);
    const char *buf = [file bytes];
    const size_t buf_len = [file length];

    plang_source_t source = plang_source_new(NULL, buf, buf_len);
    XCTAssertNotEqual(source, NULL);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    XCTAssertNotEqual(sources, NULL);
    plang_parser_t parser = plang_parser_new(sources, self.plang_log);
    XCTAssertNotEqual(parser, NULL);

    bool parsed = plang_parser_run(parser);
    plang_array_t errors = plang_parser_copy_errors(parser);
    const size_t errors_count = errors ? plang_array_get_count(errors) : 0;
    XCTAssertTrue(parsed, @"Parse failed, %zu errors", errors_count);

    for (size_t error_idx = 0; error_idx < errors_count; error_idx++) {
        plang_error_t error = plang_array_get_item(errors, error_idx);
        XCTAssertNotEqual(error, NULL);
        const char *error_text = plang_error_copy_text(error);
        XCTAssertNotEqual(error_text, NULL);
        XCTAssertTrue(false, "%zu: %s", error_idx, error_text);
        free((void *)error_text);
    }

    plang_parser_free(parser);
    plang_source_free(source);
}

- (void)testParseTrivialProgram
{
    NSError *error = nil;
    NSData *file = [self testFileWithName:@"TrivialProgram" error:&error];
    XCTAssertNotNil(file, @"Couldn't get file: %@", error);
    const char *buf = [file bytes];
    const size_t buf_len = [file length];

    plang_source_t source = plang_source_new(NULL, buf, buf_len);
    XCTAssertNotEqual(source, NULL);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    XCTAssertNotEqual(sources, NULL);
    plang_parser_t parser = plang_parser_new(sources, self.plang_log);
    XCTAssertNotEqual(parser, NULL);
    
    bool parsed = plang_parser_run(parser);
    plang_array_t errors = plang_parser_copy_errors(parser);
    const size_t errors_count = errors ? plang_array_get_count(errors) : 0;
    XCTAssertTrue(parsed, @"Parse failed, %zu errors", errors_count);

    for (size_t error_idx = 0; error_idx < errors_count; error_idx++) {
        plang_error_t error = plang_array_get_item(errors, error_idx);
        XCTAssertNotEqual(error, NULL);
        const char *error_text = plang_error_copy_text(error);
        XCTAssertNotEqual(error_text, NULL);
        XCTAssertTrue(false, "%zu: %s", error_idx, error_text);
        free((void *)error_text);
    }

    plang_parser_free(parser);
    plang_source_free(source);
}

- (void)testBase64UnitWithPreamble
{
    NSError *preambleError = nil;
    NSData *preambleData = [self testFileWithName:@"PreambleUnit"
                                            error:&preambleError];
    XCTAssertNotNil(preambleData, @"Couldn't get file: %@",
                    preambleError);
    const char *preamble_buf = [preambleData bytes];
    const size_t preamble_buf_len = [preambleData length];

    plang_source_t preamble_source
        = plang_source_new("PreambleUnit.pas",
                           preamble_buf,
                           preamble_buf_len);
    XCTAssertNotEqual(preamble_source, NULL);

    NSError *base64Error = nil;
    NSData *base64Data = [self testFileWithName:@"Base64"
                                          error:&base64Error];
    XCTAssertNotNil(base64Data, @"Couldn't get file: %@",
                    base64Error);
    const char *base64_buf = [base64Data bytes];
    const size_t base64_buf_len = [base64Data length];

    plang_source_t base64_source
        = plang_source_new("Base64.pas", base64_buf, base64_buf_len);
    XCTAssertNotEqual(base64_source, NULL);

    plang_array_t sources = plang_array_new_with_items(base64_source,
                                                       NULL);
    XCTAssertNotEqual(sources, NULL);

    plang_parser_t parser = plang_parser_new(sources, self.plang_log);
    XCTAssertNotEqual(parser, NULL);

    plang_parser_set_preamble(parser, preamble_source);

    bool parsed = plang_parser_run(parser);
    plang_array_t errors = plang_parser_copy_errors(parser);
    const size_t errors_count = errors ? plang_array_get_count(errors) : 0;
    XCTAssertTrue(parsed, @"Parse failed, %zu errors", errors_count);

    for (size_t error_idx = 0; error_idx < errors_count; error_idx++) {
        plang_error_t error = plang_array_get_item(errors, error_idx);
        XCTAssertNotEqual(error, NULL);
        const char *error_text = plang_error_copy_text(error);
        XCTAssertNotEqual(error_text, NULL);
        XCTAssertTrue(false, "%zu: %s", error_idx, error_text);
        free((void *)error_text);
    }

    plang_parser_free(parser);
    plang_source_free(preamble_source);
}

- (void)testRealNumberParser
{
    const char *buf =
    "PROGRAM RealNum;" "\n"
    "" "\n"
    "CONST" "\n"
    "    pi = 3.14159;" "\n"
    "    mol = 6.022e+23;" "\n"
    "" "\n"
    "PROCEDURE DoNothing; EXTERNAL;" "\n"
    "" "\n"
    "BEGIN" "\n"
    "    DoNothing" "\n"
    "END." "\n";
    const size_t buf_len = strlen(buf);

    plang_source_t source = plang_source_new(NULL, buf, buf_len);
    XCTAssertNotEqual(source, NULL);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    XCTAssertNotEqual(sources, NULL);
    plang_parser_t parser = plang_parser_new(sources, self.plang_log);
    XCTAssertNotEqual(parser, NULL);

    bool parsed = plang_parser_run(parser);
    plang_array_t errors = plang_parser_copy_errors(parser);
    const size_t errors_count = errors ? plang_array_get_count(errors) : 0;
    XCTAssertTrue(parsed, @"Parse failed, %zu errors", errors_count);

    for (size_t error_idx = 0; error_idx < errors_count; error_idx++) {
        plang_error_t error = plang_array_get_item(errors, error_idx);
        XCTAssertNotEqual(error, NULL);
        const char *error_text = plang_error_copy_text(error);
        XCTAssertNotEqual(error_text, NULL);
        XCTAssertTrue(false, "%zu: %s", error_idx, error_text);
        free((void *)error_text);
    }

    plang_parser_free(parser);
    plang_source_free(source);
}

@end


NS_ASSUME_NONNULL_END
