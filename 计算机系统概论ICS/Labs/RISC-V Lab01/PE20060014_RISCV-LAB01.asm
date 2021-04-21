.data
hexNum:
	.word 0x11111111,0x0000001

.text 
.global _start      # Provide program starting address to linker
_start:	
	la s0,hexNum     # Load hexNum's Address to s0
	lw t0,hexNum     # Load word to t0,so t0 <- xD108D108
	lw t1,4(s0)	 # load word from Mem[s0+4] to t1,so t1 <- 3
ENTRY:	slti t2,t0,0	 # set t2 to 1 if t0<0
	add t0,t0,t0	 # t0 <- t0+t0
	add t0,t0,t2	 # t0 <- t0+t2
	addi t1,t1,-1	 
	bnez t1,ENTRY	 # Branch to Entry if t1!=0
	sw t0,8(s0)	 # store content of t0 into Mem[s0+8]
	ret		 # return

	