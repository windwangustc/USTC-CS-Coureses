;PE20060014_LAB04 The Game of Nim        
            .ORIG x3000
            AND R1,R1,#0          ; 0 Means P1, else Means P2, P1 always goes first
            LEA R0,ROWA	    
            ADD R0,R0,#10
            ST R0,LAST_AddrA      ;store the last memory address in LABEL ROWA into LAST_AddrA
            LEA R0,ROWB 
            ADD R0,R0,#12
            ST R0,LAST_AddrB
            LEA R0,ROWC	   
            ADD R0,R0,#15
            ST R0,LAST_AddrC
            
ENTRY       LEA R0,ROWA
            LDR R0,R0,#7
            BRnp PROMPT
            LEA R0,ROWB
            LDR R0,R0,#7
            BRnp PROMPT
            LEA R0,ROWC
            LDR R0,R0,#7
            BRnp PROMPT
            BRz CHECKWINNER         ;if all the rocks in each ROW are removed,Go CHECKWINNER
            
PROMPT      LEA R0,ROWA
            PUTS
            JSR NEXTLINE
            LEA R0,ROWB
            PUTS
            JSR NEXTLINE
            LEA R0,ROWC
            PUTS
            JSR NEXTLINE
            
CHECKPLAYER ADD  R1,R1,#0           ;Check R1, if not 0, Go Player2 
            BRnp PLAYER2   
            
PLAYER1     LEA R0,P1
            PUTS
            BR  INPUT
            
PLAYER2     LEA R0,P2
            PUTS
            BR  INPUT   

INPUT       GETC                    ;Read a single Char,and the ASCII Code is copied into R0
            OUT                     ;echo the Input, we can not use IN as we don't want a default prompt.
            ST R0,InputROW
            GETC               
            OUT                 
            ST R0,InputROCKS
            JSR NEXTLINE
            
CHECKROW    LD R3,ASCII_A
            LD R4,InputROW
            ADD R3,R3,R4
            BRz INPUT_A
            LD R3,ASCII_B
            ADD R3,R3,R4
            BRz INPUT_B
            LD R3,ASCII_C
            ADD R3,R3,R4
            BRz INPUT_C
            BRnp ERROR              ;R4 failes to match ASCII A/B/C,then Go ERROR
            
INPUT_A     LD R2,NEG_RocksA         
            LD R3,LAST_AddrA
            JSR UPDATE
            ST R2,NEG_RocksA
            ST R3,LAST_AddrA
            NOT R1,R1               ;reverse R1 to change Player
            JSR NEXTLINE    
            BR ENTRY
            
INPUT_B     LD R2,NEG_RocksB         
            LD R3,LAST_AddrB
            JSR UPDATE
            ST R2,NEG_RocksB
            ST R3,LAST_AddrB
            NOT R1,R1               ;reverse R1 to change Player
            JSR NEXTLINE
            BR ENTRY
            
INPUT_C     LD R2,NEG_RocksC       
            LD R3,LAST_AddrC
            JSR UPDATE
            ST R2,NEG_RocksC 
            ST R3,LAST_AddrC
            NOT R1,R1               ;reverse R1 to change Player
            JSR NEXTLINE
            BR ENTRY
          
ERROR       LEA R0,INVALID
            PUTS
            JSR NEXTLINE
            BR CHECKPLAYER
            
CHECKWINNER ADD R1,R1,#0
            BRnp P2WINS             ;when all rocks are removed,the P1 wins if R1=0
            LEA R0,P1WIN
            PUTS
            BR EXIT
            
P2WINS      LEA R0,P2WIN
            PUTS
EXIT        HALT

UPDATE      LD R4,InputROCKS
            ADD R4,R4,#-16          ;convert inputROCKS from ASCII into decimal number
            ADD R4,R4,#-16
            ADD R4,R4,#-16
            BRnz ERROR              ;inputROCKS must be positive
            ADD R2,R2,R4
            BRp ERROR               ;inputROCKS must be less than that ROWA has
            NOT R4,R4
            ADD R4,R4,#1
            ADD R3,R3,R4            ;update the last Rock location in the ROW
            AND R5,R5,#0
            STR R5,R3,#0            ;clear the word from 'o' to '\0' at the last Rock location
            RET

NEXTLINE    ST R0,SAVER0
            LD R0,NEWLINE
            OUT
            LD R0,SAVER0
            RET
;DATA FIELD
NEG_RocksA	 .FILL xFFFD            ;Negative Rock Number in Row A, initialized as -3 
NEG_RocksB	 .FILL xFFFB            
NEG_RocksC	 .FILL xFFF8            
LAST_AddrA	 .BLKW #1
LAST_AddrB	 .BLKW #1
LAST_AddrC	 .BLKW #1
InputROW     .BLKW #1               ;stores the row the player chooses
InputROCKS   .BLKW #1               ;stores the rock number the player chooses
SAVER0       .BLKW #1   
ASCII_A	     .FILL xFFBF            ;Negative ASCII 'A' -65 
ASCII_B	     .FILL xFFBE            ;-66
ASCII_C	     .FILL xFFBD            ;-67
NEWLINE      .FILL x000A            ;ASCII for '\n'
ROWA	     .STRINGZ "ROW A: ooo"
ROWB	     .STRINGZ "ROW B: ooooo"
ROWC	     .STRINGZ "ROW C: oooooooo"
P1	         .STRINGZ "Player 1, choose a row and number of rocks: "
P2	         .STRINGZ "Player 2, choose a row and number of rocks: "
INVALID	     .STRINGZ "Invalid move. Try again."
P1WIN	     .STRINGZ "Player 1 Wins."
P2WIN	     .STRINGZ "Player 2 Wins."
             .END
            