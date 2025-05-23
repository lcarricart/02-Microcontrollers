
; P = Q + R + S
; let Q=2, R=4, S=5

		AREA	eg3, CODE, READONLY
		ENTRY
		EXPORT	__main
			
__main	PROC
		ADRL		R0, P		
		LDR			R1, [R0, #4]
		LDR			R2, [R0, #8]	
		LDR			R3, [R0, #12]	
		
		ADD			R0, R1, R2
		ADD			R0, R0, R3	
		
STOP	B			STOP
		
;////// All this data is now placed below the main because apparently that matters to solve a compiling Error. ///////		

		AREA	eg3, DATA, READONLY
		
P		SPACE		4
Q		DCD			2 
R		DCD			4
S		DCD			5
		ALIGN
;////// All this data is now placed below the main because apparently that matters to solve a compiling Error. ///////
		
		
		ENDP
		
