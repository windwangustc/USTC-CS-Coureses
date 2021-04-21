; Unfortunately we have not YET installed Windows or Linux on the LC-3,
; so we are going to have to write some operating system code to enable
; keyboard interrupts. The OS code does three things:
;
;    (1) Initializes the interrupt vector table with the starting
;        address of the interrupt service routine. The keyboard
;        interrupt vector is x80 and the interrupt vector table begins
;        at memory location x0100. The keyboard interrupt service routine
;        begins at x1000. Therefore, we must initialize memory location
;        x0180 with the value x1000.
;    (2) Sets bit 14 of the KBSR to enable interrupts.
;    (3) Pushes a PSR and PC to the system stack so that it can jump
;        to the user program at x3000 using an RTI instruction.

        .ORIG x800
        ; (1) Initialize interrupt vector table.
        LD R0, VEC
        LD R1, ISR
        STR R1, R0, #0

        ; (2) Set bit 14 of KBSR.
        LDI R0, KBSR
        LD R1, MASK
        NOT R1, R1
        AND R0, R0, R1
        NOT R1, R1
        ADD R0, R0, R1
        STI R0, KBSR

        ; (3) Set up system stack to enter user space.
        LD R0, PSR
        ADD R6, R6, #-1
        STR R0, R6, #0
        LD R0, PC
        ADD R6, R6, #-1
        STR R0, R6, #0
        ; Enter user space.
        RTI
        
VEC     .FILL x0180
ISR     .FILL x1000
KBSR    .FILL xFE00
MASK    .FILL x4000
PSR     .FILL x8002
PC      .FILL x3000
        .END

            .ORIG x3000
            ; *** Begin user program code here ***
LOOP        LEA R0,ICS2020          
            PUTS
            JSR DELAY
            BR LOOP
            HALT
ICS2020     .STRINGZ "ICS2020 "
            ;*** Delay Subroutine ***
DELAY		ST R1,Save_R1
		    LD R1,COUNT
REP1		ADD R1,R1,#-1
		    BRp REP1
		    LD R1,COUNT
REP2		ADD R1,R1,#-1           ;Repeat count once more to delay longer
		    BRp REP2
		    LD R1,Save_R1
		    RET
COUNT		.FILL x7FFF
Save_R1		.BLKW #1
            .END

            .ORIG x1000
            ; *** Begin interrupt service routine code here ***
            ST R0,SaveR0
            ST R1,SaveR1
            ST R7,SaveR7
	        LD R0,NEWLINE
            OUT
CHECK       LDI R0,_KBDR            ;to check if KBDR is within the decimal num range
	        LD  R1,NEG_x30          ; RO should be within x30~x39
	        ADD R1,R1,R0
	        BRn OUTRANGE
	        LD  R1,NEG_x39 
	        ADD R1,R1,R0
            BRp OUTRANGE
	        OUT
	        LEA R0 DIGTALPMT
            PUTS 
	        BR DONE
OUTRANGE    OUT
            LEA R0 NONDIGTALPMT
            PUTS 
DONE        LD R0,NEWLINE
            OUT
            LD R0,SaveR0
            LD R1,SaveR1
            LD R7,SaveR7
            RTI
DIGTALPMT     .STRINGZ " is a decimal digit."
NONDIGTALPMT  .STRINGZ " is not a decimal digit."
NEWLINE      .FILL x0A    
NEG_x30      .FILL xFFCF        ;nagetive x30  (ASCII of 0)
NEG_x39      .FILL xFFC6        ;nagetive x39  (ASCII of 9) 
SaveR0		 .BLKW #1
SaveR1		 .BLKW #1
SaveR7		 .BLKW #1
_KBSR        .FILL xFE00
_KBDR	     .FILL XFE02
             .END