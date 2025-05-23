		AREA	Swap, CODE, READONLY
		ENTRY
		EXPORT  __main

__main   PROC
		LDR		R0, =0xBABEFACE
		LDR		R1, =0xDEADBEEF
		
		EOR		R0,R0,R1 
		EOR		R1,R0,R1
		EOR		R0,R0,R1
		
		;Simple routine to swap values through the "Exclusive OR"		
STOP	B		STOP
	
		ENDP
		END