{
    TrivialProgram.pas
    plang Tests

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
}

PROGRAM Hello;

TYPE
    { Any type of string. }
    AnyStr = STRING[255];

{ Write a line of text to standard output. }
PROCEDURE writeln(line: AnyStr); EXTERNAL;


BEGIN
    writeln('Hello, world!')
END.
