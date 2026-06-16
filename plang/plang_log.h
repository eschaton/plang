/*
    plang_log.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_log__h__
#define __plang_log__h__

#include "plang_defines.h"

#include <stdarg.h>

PLANG_HEADER_BEGIN


/*! A log in `plang`. */
typedef struct plang_log_s *plang_log_t;


/*!
 A log level.

 The levels of logging available, in order of priority.
 */
typedef enum {
    plang_log_level_debug = 0,
    plang_log_level_info,
    plang_log_level_notice,
    plang_log_level_warning,
    plang_log_level_error,
} plang_log_level_t;


/*!
 A log output function.

 The log output function is what actually records a log message for a
 level. It's passed the \a context that was passed when creating the
 ``plang_log_t`` with which it's associated. It's also passed the level
 of indentation in use, to implement as it will.
 */
typedef void (*plang_log_output_t)(const plang_log_level_t level,
                                   const char *message,
                                   int indent,
                                   void *context);


plang_log_t PLANG_NULLABLE
plang_log_new(plang_log_output_t output_function,
              void * PLANG_NULLABLE context);


void
plang_log_free(plang_log_t PLANG_NULLABLE log);


/*!
 Set the default log level.

 Sets the minimum level to log. Initially this is set to log notices,
 warnings, and errors (``plang_log_level_notice``).
 */
void
plang_log_set_default_level(plang_log_t log,
                            plang_log_level_t level);


/*! Get the default log level. */
plang_log_level_t
plang_log_get_default_level(plang_log_t log);


/*! Log at the given level (composable). */
void
plang_logv(plang_log_t log,
           plang_log_level_t level,
           const char *fmt,
           va_list args);


/*! Log at the given level (varargs). */
void
plang_log(plang_log_t log,
          plang_log_level_t level,
          const char *fmt,
          ...);


/*! Log at the given level and increase indentation (composable). */
void
plang_log_indentv(plang_log_t log,
                  plang_log_level_t level,
                  const char *fmt,
                  va_list args);


/*! Log at the given level and increase indentation (varargs). */
void
plang_log_indent(plang_log_t log,
                 plang_log_level_t level,
                 const char *fmt,
                 ...);


/*! Decrease indentation and log at the given level (composable). */
void
plang_log_outdentv(plang_log_t log,
                   plang_log_level_t level,
                   const char *fmt,
                   va_list args);


/*! Decrease indentation and log at the given level (varargs). */
void
plang_log_outdent(plang_log_t log,
                  plang_log_level_t level,
                  const char *fmt,
                  ...);


PLANG_HEADER_END

#endif /* __plang_function__h__ */
