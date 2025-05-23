
; P = Q + R + S
; let Q=2, R=4, S=5


Q		EQU		2 ;Q equals 2
R		EQU		4
S		EQU		5
	
		AREA	SimpleEq1, CODE, READONLY
		ENTRY
		EXPORT	__main
			
__main
		MOV		R1, #Q
		MOV		R2, #R
		MOV		R3, #S
		
		ADD		R0, R1, R2
		ADD		R0, R0, R3
		
STOP	B		STOP		;Commenting this line out would step you out of the main while using the debugger, entering the startup file
							;and others again. I might not be able to see this due to the inproper compiler's behavior on my computer.

		END
			
		