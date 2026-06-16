/*
    plang_log.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang_log_internal.h"

#include <stdio.h>
#include <stdlib.h>

PLANG_SOURCE_BEGIN


#ifndef PLANG_LOG_BUFFER_SIZE
#define PLANG_LOG_BUFFER_SIZE 32768
#endif


plang_log_t PLANG_NULLABLE
plang_log_new(plang_log_output_t output_function,
              void * PLANG_NULLABLE context)
{
    struct plang_log_s *log = NULL;

    log = calloc(sizeof(struct plang_log_s), 1);
    if (log == NULL) return NULL;

    log->_default_level = plang_log_level_notice;
    log->_output_function = output_function;
    log->_context = context;
    log->_buffer = calloc(sizeof(char), PLANG_LOG_BUFFER_SIZE);
    log->_indent = 0;

    if (log->_buffer == NULL) goto error;

    return log;

error:
    plang_log_free(log);
    return NULL;
}


void
plang_log_free(plang_log_t PLANG_NULLABLE log)
{
    if (log == NULL) return;

    free(log->_buffer);

    free(log);
}


void
plang_log_set_default_level(plang_log_t log,
                            plang_log_level_t level)
{
    log->_default_level = level;
}


plang_log_level_t
plang_log_get_default_level(plang_log_t log)
{
    return log->_default_level;
}


void
plang_logv(plang_log_t log,
           plang_log_level_t level,
           const char *fmt,
           va_list args)
{
    if (level < log->_default_level) return;

    vsnprintf(log->_buffer, PLANG_LOG_BUFFER_SIZE, fmt, args);

    log->_output_function(level,
                          log->_buffer,
                          log->_indent,
                          log->_context);
}


void
plang_log(plang_log_t log,
          plang_log_level_t level,
          const char *fmt,
          ...)
{
    if (level < log->_default_level) return;

    va_list ap;

    va_start(ap, fmt);
    plang_logv(log, level, fmt, ap);
    va_end(ap);
}


void
plang_log_indentv(plang_log_t log,
                  plang_log_level_t level,
                  const char *fmt,
                  va_list args)
{
    if (level < log->_default_level) return;

    plang_logv(log, level, fmt, args);

    log->_indent += 1;
}


void
plang_log_indent(plang_log_t log,
                 plang_log_level_t level,
                 const char *fmt,
                 ...)
{
    if (level < log->_default_level) return;

    va_list ap;

    va_start(ap, fmt);
    plang_log_indentv(log, level, fmt, ap);
    va_end(ap);
}


void
plang_log_outdentv(plang_log_t log,
                   plang_log_level_t level,
                   const char *fmt,
                   va_list args)
{
    if (level < log->_default_level) return;

    log->_indent -= 1;

    plang_logv(log, level, fmt, args);
}


void
plang_log_outdent(plang_log_t log,
                  plang_log_level_t level,
                  const char *fmt,
                  ...)
{
    if (level < log->_default_level) return;

    va_list ap;

    va_start(ap, fmt);
    plang_log_outdentv(log, level, fmt, ap);
    va_end(ap);
}


PLANG_SOURCE_END
