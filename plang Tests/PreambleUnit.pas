{
    PreambleUnit.pas
    plang Tests

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.

    This file should be kept identical to plang.pas.
}

UNIT plang;

INTERFACE

{ USES no other units. }

{ Default Constants }

CONST
    MAXINT8 = 127;
    MAXINT16 = 32767;
    MAXINT32 = 2147483647;
    MAXINT64 = 9223372036854775807;


{ Default Types }

TYPE
    INT8 = -128..127;
    INT16 = -32768..32767;
    INT32 = -2147483648..2147483647;
    INT64 = -9223372036854775808..9223372036854775807;
    CHAR = INT8;
	BYTE = INT8;
    INTEGER = INT16;
    LONGINT = INT32;
    BOOLEAN = ( FALSE, TRUE );
    ANYPTR = ^BYTE;


{ No default variables yet. }


{ Default Procedures }

PROCEDURE NEW(VAR P: ANYPTR); EXTERNAL;
PROCEDURE DISPOSE(VAR P: ANYPTR); EXTERNAL;


{ Default Functions }

FUNCTION CHR(i8: INT8): CHAR; EXTERNAL;
FUNCTION ORD(ch: CHAR): INT8; EXTERNAL;

FUNCTION POINTER(i64: INT64): ANYPTR; EXTERNAL;
FUNCTION ORDPTR(p: ANYPTR): INT64; EXTERNAL;

FUNCTION BIT_AND(a, b: INT64): INT64; EXTERNAL;
FUNCTION BIT_OR(a, b: INT64): INT64; EXTERNAL;
FUNCTION BIT_NOT(a: INT64): INT64; EXTERNAL;

FUNCTION BIT_SHL(a: INT64; by: INT8): INT64; EXTERNAL;
FUNCTION BIT_SHR(a: INT64; by: INT8): INT64; EXTERNAL;


IMPLEMENTATION

{ All implementation is external. }

END.
