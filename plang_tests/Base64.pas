{
    Base64.pas
    plang Tests

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
}

UNIT Base64;

{ An API for Base64 encoding and decoding. }
{ TODO: Handle trailing '=' appropriately for data size. }


INTERFACE

TYPE
    B64EncPtr = ^CHAR; { Pointer to encoded data. }
    B64DecPtr = ^BYTE; { Pointer to decoded data. }
    B64Size = LONGINT;

{ Initialize the base64 CODEC. }
PROCEDURE B64Initialize;

{ Finalize the base64 CODEC. }
PROCEDURE B64Finalize;

{ Encode input data into base64 output characters. }
PROCEDURE B64Encode(input: B64DecPtr;
                    inSize: B64Size;
                    output: B64EncPtr;
                    outSize: B64Size);

{ Decode base64 input characters into an output data buffer. }
PROCEDURE B64Decode(input: B64EncPtr;
                    inSize: B64Size;
                    output: B64DecPtr;
                    outSize: B64Size);


IMPLEMENTATION

CONST
    decSize = 3;
    encSize = 4;

TYPE
    { A buffer of encoded characters. }
    B64EncBuf = ARRAY [0..3] OF CHAR;

    { A buffer of decoded bytes. }
    B64DecBuf = ARRAY [0..2] OF BYTE;

    { Convenience for converting from ^BYTE to array. }
    ByteArr = ARRAY [0..2] OF BYTE;
    ByteArrPtr = ^ByteArr;

    { Convenience for converting from ^CHAR to array. }
    CharArr = ARRAY [0..3] OF CHAR;
    CharArrPtr = ^CharArr;

    { Type of the encoding table itself. }
    EncTab = ARRAY[0..63] OF CHAR;

    { Type of the decoding table itself. }
    DecTab = ARRAY[0..255] OF BYTE;

VAR
    gEncTab: ^EncTab;       { The encoding table. }
    gDecTab: ^DecTab;       { The decoding table. }


PROCEDURE B64Initialize;
    PROCEDURE SetUpET;
    VAR
        i: BYTE;

        { Compute the encoding table. }
        FUNCTION B64EncChar(b: BYTE): CHAR;
        BEGIN
            IF b IN [0..25]  THEN B64EncChar := CHR(ORD('A') + b);
            IF b IN [26..51] THEN B64EncChar := CHR(ORD('a') + b);
            IF b IN [52..61] THEN B64EncChar := CHR(ORD('0') + b);
            IF b = 62        THEN B64EncChar := '+';
            IF b = 63        THEN B64EncChar := '/'
        END;
    BEGIN
        FOR i := 0 TO 63 DO
            gEncTab^[i] := B64EncChar(i)
    END;

    PROCEDURE SetUpDT;
    VAR
        i: INTEGER;

        { Compute the decoding table. }
        FUNCTION B64DecValue(ch: INTEGER): BYTE;
        BEGIN
            IF CHR(ch) IN ['A'..'Z'] THEN
                B64DecValue := ch - ORD('A') + 0;
            IF CHR(ch) IN ['a'..'z'] THEN
                B64DecValue := ch - ORD('a') + 26;
            IF CHR(ch) IN ['0'..'9'] THEN
                B64DecValue := ch - ORD('0') + 52;
            IF CHR(ch) = '+' THEN
                B64DecValue := 62;
            IF CHR(ch) = '/' THEN
                B64DecValue := 63;
            IF CHR(ch) = '=' THEN
                B64DecValue := 0;
            B64DecValue := 0
        END;
    BEGIN
        FOR i := 0 TO 255 DO
            gDecTab^[i] := B64DecValue(i)
    END;

BEGIN
    NEW(gEncTab);
    NEW(gDecTab);

    SetUpET;
    SetUpDT
END;


PROCEDURE B64Finalize;
BEGIN
    DISPOSE(gDecTab);
    gDecTab := NIL;

    DISPOSE(gEncTab);
    gEncTab := NIL
END;

{ Encode one 3-btye chunk of data to 4 characters. }
PROCEDURE B64EncOne(    input: B64DecBuf;
                    VAR output: B64EncBuf);

VAR
    indexes: ARRAY [0..3] OF BYTE;

    { Compute the indexes into the encoding table. }
    PROCEDURE MakeIndexes;
    CONST
        mask7thru2 = $FC; { 11111100 }
        mask7thru4 = $F0; { 11110000 }
        mask7thru6 = $C0; { 11000000 }
        mask5thru0 = $3F; { 00111111 }
        mask3thru0 = $0F; { 00001111 }
        mask1thru0 = $03; { 00000011 }
    VAR
        inputbs: ARRAY [0..2] OF BYTE;
    BEGIN
        {$R-}

        { get types out of the way }
        inputbs[0] := ORD(input[0]);
        inputbs[1] := ORD(input[1]);
        inputbs[2] := ORD(input[2]);

        { (input[0] & 0xFC) >> 2; }
        indexes[0] := BIT_SHR(BIT_AND(inputbs[0], mask7thru2), 2);

        { ((input[0] & 0x03) << 6) | ((input[1] & 0xF0) >> 4); }
        indexes[1] := BIT_OR(BIT_SHL(BIT_AND(inputbs[0], mask1thru0), 6),
                             BIT_SHR(BIT_AND(inputbs[1], mask7thru4), 4));

        { ((input[1] & 0x0F) << 2) | ((input[2] & 0xC0) >> 6); }
        indexes[2] := BIT_OR(BIT_SHL(BIT_AND(inputbs[1], mask3thru0), 2),
                             BIT_SHR(BIT_AND(inputbs[2], mask7thru6), 6));

        { input[2] & 0x3F; }
        indexes[3] := BIT_AND(inputbs[2], mask5thru0)

        {$R+}
    END;

BEGIN
    MakeIndexes;

    { populate the output }
    {$R-}
    output[0] := gEncTab^[indexes[0]];
    output[1] := gEncTab^[indexes[1]];
    output[2] := gEncTab^[indexes[2]];
    output[3] := gEncTab^[indexes[3]]
    {$R+}
END;


{ Decode one 4-character chunk of data to 3 bytes. }
PROCEDURE B64DecOne(    input: B64EncBuf;
                    VAR output: B64DecBuf);
VAR
    decValues: ARRAY [0..3] OF BYTE;
BEGIN
    {$R-}
    decValues[0] := gDecTab^[ORD(input[0])];
    decValues[1] := gDecTab^[ORD(input[1])];
    decValues[2] := gDecTab^[ORD(input[2])];
    decValues[3] := gDecTab^[ORD(input[3])];

    { output[0] = ((inval[0] & 0x3f) << 2) | ((inval[1] & 0x30) >> 4); }
    output[0] := BIT_OR(BIT_SHL(BIT_AND(decValues[0], $3F), 2),  { 0b00111111 }
                        BIT_SHR(BIT_AND(decValues[1], $30), 4)); { 0b00110000 }

    { output[1] = ((inval[1] & 0x0f) << 4) | ((inval[2] & 0x3c) >> 2); }
    output[1] := BIT_OR(BIT_SHL(BIT_AND(decValues[1], $0F), 4),  { 0b00001111 }
                        BIT_SHR(BIT_AND(decValues[2], $3C), 2)); { 0b00111100 }

    { output[2] = ((inval[2] & 0x03) << 6) | ((inval[3] & 0x3f) >> 0); }
    output[2] := BIT_OR(BIT_SHL(BIT_AND(decValues[2], $03), 6),  { 0b00000011 }
                        BIT_SHR(BIT_AND(decValues[3], $3F), 0))  { 0b00111111 }
    {$R+}
END;


PROCEDURE B64Encode(input: B64DecPtr;
                    inSize: B64Size;
                    output: B64EncPtr;
                    outSize: B64Size);
VAR
    inIndex: B64Size;
    outIndex: B64Size;
    encoded: B64EncBuf;
    decoded: B64DecBuf;
    inputPtr: ByteArrPtr;
    outputPtr: CharArrPtr;

BEGIN
    {$R-}
    inIndex := 0;
    outIndex := 0;

    { Stride through the input 3 bytes at a time
      and the output 4 chars at a time, encoding. }
    REPEAT
        inputPtr := POINTER(ORDPTR(input) + inIndex);
        decoded[0] := inputPtr^[0];
        decoded[1] := inputPtr^[1];
        decoded[2] := inputPtr^[2];

        B64EncOne(decoded, encoded);

        outputPtr := POINTER(ORDPTR(output) + outIndex);
        outputPtr^[outIndex] := encoded[0];
        outputPtr^[outIndex] := encoded[1];
        outputPtr^[outIndex] := encoded[2];
        outputPtr^[outIndex] := encoded[3];

        inIndex := inIndex + 3;
        outIndex := outIndex + 4
    UNTIL ((inIndex >= inSize) OR (outIndex >= outSize))
    {$R+}
END;


PROCEDURE B64Decode(input: B64EncPtr;
                    inSize: B64Size;
                    output: B64DecPtr;
                    outSize: B64Size);
VAR
    inIndex: B64Size;
    outIndex: B64Size;
    encoded: B64EncBuf;
    decoded: B64DecBuf;
    inputPtr: CharArrPtr;
    outputPtr: ByteArrPtr;

BEGIN
    {$R-}
    inIndex := 0;
    outIndex := 0;

    { Stride through the input 4 chars at a time
      and the output 3 bytes at a time, decoding. }
    REPEAT
        inputPtr := POINTER(ORDPTR(input) + inIndex);
        encoded[0] := inputPtr^[0];
        encoded[1] := inputPtr^[1];
        encoded[2] := inputPtr^[2];
        encoded[3] := inputPtr^[3];

        B64DecOne(encoded, decoded);

        outputPtr := POINTER(ORDPTR(output) + outIndex);
        outputPtr^[0] := decoded[0];
        outputPtr^[1] := decoded[1];
        outputPtr^[2] := decoded[2];

        inIndex := inIndex + 4;
        outIndex := outIndex + 3
    UNTIL ((inIndex >= inSize) OR (outIndex >= outSize))
    {$R+}
END;


END.
