
				AREA	simpleShift, CODE, READONLY
				ENTRY
				EXPORT __main
				
__main
		MOV		R0, #0x11			; = 17 or HEX 0x11
		LSL		R1, R0, #1			;Gonna shift the content of R0 one bit to the left, storing it in R1. = 34 or HEX 0x22
		LSL		R2, R1, #1			; = 68 or HEX 0x44
		
STOP	B		STOP
		
		END
		
