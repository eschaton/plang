/*
    plang_log_internal.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_log_internal__h__
#define __plang_log_internal__h__

#include "plang_log.h"

PLANG_HEADER_BEGIN


/*!
 Internal structure of a ``plang_log_t``.

 Named ``plang_log_s`` so as to avoid confusion with the log function,
 which is named ``plang_log``.
 */
struct plang_log_s {
    plang_log_level_t _default_level;
    plang_log_output_t _output_function;
    void * PLANG_NULLABLE _context;
    char *_buffer;
    int _indent;
};


PLANG_HEADER_END

#endif /* __plang_log_internal__h__ */
