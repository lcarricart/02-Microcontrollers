
; P = Q + R + S
; let Q=2, R=4, S=5

		AREA	eg3, CODE, READONLY
		ENTRY
		EXPORT	__main
			
__main
		ADRL		R4, Vals		;When we point at an address out of range we use ADRL, otherwise just ADR.
									;If we ever get an "Out of range" error, use ADRL
		LDR			R1, [R4, #P1]	;Loads the value at base address + P1 (which is 2) into R1
		LDR			R2, [R4, #P2]	
		LDR			R3, [R4, #P3]	
		
		ADD			R0, R1, R2
		ADD			R0, R0, R3
		
		STR			R0, [R4, #P0]	;First operand is the source, the second the destination, the third the offset. This for STR instruction.
									;P0 plays an exaplanatory role here, since its value is 0 and it isn't necessary.
		
STOP	B			STOP
		
;////// All this data is now placed below the main because apparently that matters to solve a compiling Error. ///////		
P0		EQU		0
P1		EQU		4
P2		EQU		8
P3		EQU		12
;We'll use pointers to modify their values
	
		AREA	eg3, DATA, READONLY
		
Vals	SPACE		4
		;We reserve spaces in memory with specific values. To access them we'll need to stablish offsets, moving through memory areas.
		;These spaces are separated by 4 bytes, and this is why P0=0, P1=4,...
		DCD			2 
		DCD			4
		DCD			5
		ALIGN
;////// All this data is now placed below the main because apparently that matters to solve a compiling Error. ///////
		
		
		END
		
