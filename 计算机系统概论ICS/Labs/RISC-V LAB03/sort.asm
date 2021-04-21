.data
FirstAddr:	.word 0x10010020    	 # address of headnode
.text 
.global _start                     	 # Provide program starting address to linker
_start:		
		lw t0,FirstAddr	   	 # load first address of the linklist 
		lw t1,(t0)
		beqz t1,DONE	   	 # exit if the first address is x00000000
		
IntLoop:	lw t1,(t0)	   	 # t1=t0->nextaddr
Load:		lw t2,4(t0)	   	 # t2=t0->value
		lw t3,4(t1)		 # t3=t1->value
		bgt t2,t3,SWAP           # Swap if t2>t3
NextAddr:	lw t1,(t1)		 # t1=t1->nextaddr
		beqz t1,ExtLoop          
		b  Load
		
SWAP:		sw t3,4(t0)
		sw t2,4(t1)
		b  NextAddr
		
ExtLoop:	lw t0,(t0)		 # t0=t0->nextaddr
		bnez t0,IntLoop	
DONE:	        ret
