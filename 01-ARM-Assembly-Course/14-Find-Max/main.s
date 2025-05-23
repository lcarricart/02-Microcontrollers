
COUNT	RN		R0
MAX		RN		R1
POINTER	RN		R2
NEXT	RN		R3


			AREA	myData, DATA, READONLY
MYDATA		DCD		69,87, 96, 45, 75
	
			AREA	myCode, CODE, READONLY
			ENTRY
			EXPORT __main
				
__main		PROC	
			MOV COUNT, #5
			MOV	MAX, #0
			LDR	POINTER, =MYDATA
			LDR NEXT, [POINTER]
			CMP	MAX, NEXT
			BHS	CTNU		;Subroutine
			MOV	MAX, NEXT
			
CTNU		ADD	POINTER, POINTER, #4	;After checking one number, there're four left to be checked.
			SUBS COUNT,	COUNT, #1		;3, 2, 1, 0
			BNE	AGAIN					;Continue if the value is =! 0
			
STOP		B	STOP
			
			ENDP
			END
			
			