/*
    plang_source.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang_source_internal.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PLANG_SOURCE_BEGIN


plang_source_t PLANG_NULLABLE
plang_source_new(const char * PLANG_NULLABLE vpath,
                 const char *buf,
                 const size_t buf_len)
{
    plang_source_t source = calloc(sizeof(struct plang_source), 1);
    if (source) {
        if (vpath) {
            source->_vpath = strdup(vpath);
        } else {
            source->_vpath = plang_source_create_vpath(buf, buf_len);
        }
        if (source->_vpath == NULL) goto error;

        char *newbuf = calloc(sizeof(char), buf_len);
        if (newbuf == NULL) goto error;

        memcpy(newbuf, buf, buf_len);

        source->_buf = newbuf;
        source->_buf_len = buf_len;
    }

    return source;

error:
    plang_source_free(source);
    return NULL;
}


plang_source_t PLANG_NULLABLE
plang_source_new_from_file(const char *path)
{
    plang_source_t preamble = NULL;

    char *rpath = realpath(path, NULL);
    if (rpath != NULL) {
        FILE *file = fopen(rpath, "r");
        if (file != NULL) {
            int seek_success = fseeko(file, 0, SEEK_END);
            if (seek_success != -1) {
                off_t len = ftello(file);
                if (len != -1) {
                    (void) fseeko(file, 0, SEEK_SET);
                    size_t buf_len = (size_t) len;
                    char *buf = calloc(sizeof(char), buf_len);
                    if (buf != NULL) {
                        ssize_t items = fread(buf, len, 1, file);
                        if (items == 1) {
                            preamble
                                = plang_source_new(rpath, buf, len);
                        }
                        
                        free(buf);
                    }
                }
            }

            fclose(file);
        }
        
        free(rpath);
    }

    return preamble;
}


void
plang_source_free(plang_source_t PLANG_NULLABLE source)
{
    if (source) {
        free((void *) source->_vpath);
        free((void *) source->_buf);
        free(source);
    }
}


const char *
plang_source_get_vpath(plang_source_t source)
{
    return source->_vpath;
}


size_t
plang_source_get_length(plang_source_t source)
{
    return source->_buf_len;
}


char
plang_source_get_char(plang_source_t source,
                      size_t position)
{
    if (position < source->_buf_len) {
        return source->_buf[position];
    } else {
        return '\0';
    }
}


const char * PLANG_NULLABLE
plang_source_get_chars(plang_source_t source,
                       size_t position)
{
    if (position < source->_buf_len) {
        return &source->_buf[position];
    } else {
        return NULL;
    }
}


const char * PLANG_NULLABLE
plang_source_create_vpath(const char *buf,
                          const size_t buf_len)
{
    // TODO: Implement this for real.

    /* Do something that's unique enough for now. */

    char result[PATH_MAX];

    snprintf(result, PATH_MAX, "vpath:%p-%zX", buf, buf_len);

    return strdup(result);
}


PLANG_SOURCE_END
