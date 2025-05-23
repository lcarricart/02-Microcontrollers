
			AREA		myCode, CODE, READONLY
			ENTRY
			EXPORT		__main
				
__main
			MOV			R2, #4
			MOV			R3, #2
			MOV			R4, #4
			
			SUBS		R5, R2, R3		;R5=R2-R3 (4 - 2 = 2) 
			SUBS		R5, R2, R4		;(4 - 4 = 0)
			;With just the SUB instruction, I wouldn't be able to read the flags.
			
STOP		B			STOP

			END
			