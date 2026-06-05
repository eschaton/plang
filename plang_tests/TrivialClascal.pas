UNIT TrivialClascal;

INTERFACE

TYPE
    CLASSREF = __BUILT_IN_CLASSREF;
    Object =
            SUBCLASS OF NIL
                FUNCTION New: Object;
                PROCEDURE Free;
            END;

IMPLEMENTATION

{ Create an instance of the given class. }
FUNCTION NewObject(c: CLASSREF): Object; EXTERNAL;

{ Free the given object instance. }
PROCEDURE FreeObject(o: Object); EXTERNAL;

METHODS OF Object;
    FUNCTION Object.New: Object;
    BEGIN
        SELF := NewObject(SELFCLASS);
        New := SELF
    END;

    PROCEDURE Object.Free;
    BEGIN
        FreeObject(SELF)
    END;
END;

END.
