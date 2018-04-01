. TEST PROGRAM
COPY    START   0 Comment here
FIRST   STL     RETADR  Comment here
CLOOP   JSUB    RDREC  Comment here
        LDA     LENGTH  Comment here
        COMP    #0  Comment here
        JEQ     ENDFIL  Comment here
        JSUB    WRREC  Comment here
        J       CLOOP Comment here
ENDFIL  LDA     =C'EOF' Comment here
        STA     BUFFER Comment here
        LDA     #3  Comment here
        STA     LENGTH  Comment here
        JSUB    WRREC  Comment here
        J       @RETADR Comment here
        USE     CDATA
RETADR  RESW    1 Comment here
LENGTH  RESW    1 Comment here
        USE     CBLKS
BUFFER  RESB    4096 Comment here
BUFEND  EQU     * Comment here to test * literal
MAXLEN  EQU     BUFEND-BUFFER
.
.   RDREC SUBROUTINE
.
        USE     DEFAULT
RDREC   CLEAR   X Comment here
        CLEAR   A Comment here
        CLEAR   S Comment here
        +LDT    #MAXLEN Comment here
RLOOP   TD      INPUT Comment here
        JEQ     RLOOP Comment here
        RD      INPUT Comment here
        COMPR   A,S Comment here
        JEQ     EXIT Comment here
        STCH    BUFFER,X Comment here
        TIXR    T Comment here
        JLT     RLOOP Comment here
EXIT    STX     LENGTH Comment here
        RSUB    Comment here
        USE     CDATA
INPUT   BYTE    X'F1' Comment here
.
.   WRREC SUBROUTINE
.
        USE     DEFAULT
WRREC   CLEAR   X Comment here
        LDT     LENGTH Comment here
WLOOP   TD      =X'05' Comment here
        JEQ     WLOOP Comment here
        LDCH    BUFFER,X Comment here
        WD      =X'05' Comment here
        TIXR    T Comment here
        JLT     WLOOP Comment here
        RSUB         Comment here
        USE     CDATA
        LTORG
        END     FIRST Comment here
