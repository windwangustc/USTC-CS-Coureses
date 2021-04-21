.data
hexNum1:
	           .word 0x00000ae9
hexNum2:		
		   .word 0x00000001

.text 
.global _start                      # Provide program starting address to linker
_start:	
	           lw t1,hexNum1     # Load 2 hexNumber into t0 and t1
	           lw t2,hexNum2     
CHECKEQL:          beq t1,t2,DONE    # return if t0 = t1
	           blt t1,t2,MINUS   # t2=t2-t1 if t1 < t2
	           sub t1,t1,t2      # t1=t1-t2 if t1 > t2
	           b   CHECKEQL
MINUS:		   sub t2,t2,t1
		   b   CHECKEQL
DONE:	           ret		      # return and t1 saves the result GCD
