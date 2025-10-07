// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
// The algorithm is based on repetitive addition.

//// *Indentation is for my own logical understanding

// variables
@i 
M = 1
@nR1 
M = 0 
@nR2 
M = 0

// check if anyone of R0 or R1 = 0
@R0 
D = M 
@ZERO
D; JEQ 
@R1 
D = M 
@ZERO 
D; JEQ 

(CHECK)
    @R0 
    D = M 
    @MAKER0POS
    D; JLT 

    @R1 
    D = M 
    @MAKER1POS 
    D; JLT 

@R0 
D = M 
@c // variable c is the copy of |R0|
M = D
@CAL 
0; JMP 

(ZERO)
    @R2 
    M = 0
    @END 
    0; JMP 

(MAKER0POS)
    @nR0
    M = 1
    @R0 
    M = -M 
    @CHECK 
    0; JMP 

(MAKER1POS)
    @nR1 
    M = 1
    @R1 
    M = -M 
    @CHECK 
    0; JMP 

(LOOP)
    @c 
    D = M 
    @R0 
    M = D + M 
    @i 
    M = M + 1 
    @CAL 
    0; JMP 
(CAL)
    @i 
    D = M 
    @R1 
    D = D - M 
    @LOOP
    D; JLT
    @WRITER2
    0; JMP 

(WRITER2) 
    @R0 
    D = M 
    @R2 
    M = D

    // check if ans should be -ve 
    @nR0 
    D = M 
    @nR1 
    D = D + M 
    D = D - 1
    @NEGATE 
    D; JEQ 

    @END
    0; JMP 

(NEGATE)
    @R2 
    M = -M 

(END)
    @END
    0; JMP
    