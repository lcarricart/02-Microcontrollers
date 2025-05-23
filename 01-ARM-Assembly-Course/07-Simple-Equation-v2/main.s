
; P = Q + R + S
; let Q=2, R=4, S=5

			AREA		SimpleAddData, DATA, READONLY
	
		SPACE	4		;Reserve 4 bytes of memoery
Q		DCD		2		;Create a 32-bits variable Q with an initial value of 2, Q = 0x00000002
R		DCD		4
S		DCD		5

			AREA		SimpleAddCode, CODE, READONLY
			ENTRY
			EXPORT		__main
				
__main	
			LDR			R1, Q
			LDR			R2, R
			LDR			R3, S
			
			ADD			R0, R1, R2
			ADD			R0, R0, R3

STOP		B			STOP

			END