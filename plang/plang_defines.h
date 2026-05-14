/*
    plang_defines.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_defines__h__
#define __plang_defines__h__


#define PLANG_NULLABLE		_Nullable
#define PLANG_NONNULL		_Nonnull

#define PLANG_NONNULL_BEGIN _Pragma("clang assume_nonnull begin")
#define PLANG_NONNULL_END   _Pragma("clang assume_nonnull end")

#define PLANG_HEADER_BEGIN  PLANG_NONNULL_BEGIN
#define PLANG_HEADER_END    PLANG_NONNULL_END

#define PLANG_SOURCE_BEGIN  PLANG_NONNULL_BEGIN
#define PLANG_SOURCE_END    PLANG_NONNULL_END


#endif /* __plang_defines__h__ */
