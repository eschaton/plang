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

        source->_buf = strdup(buf);
        source->_buf_len = buf_len;
    }

    return source;
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
