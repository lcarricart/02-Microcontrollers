COUNT	RN		R0
MAX		RN		R1
POINTER	RN		R2
NEXT	RN		R3
MIN		RN		R4


			AREA	myData, DATA, READONLY
MYDATA		DCD		69, 87, 96, 45, 75			;Is an array
	
			AREA	myCode, CODE, READONLY
			ENTRY
			EXPORT __main
				
__main		PROC
			MOV COUNT, #5
			MOV	MAX, #0
			MOV	MIN, #1000
			LDR	POINTER, =MYDATA				;Load the address of MYDATA into POINTER. It uses the base address (first value of the array).

AGAIN		LDR NEXT, [POINTER]					;Loop starts. Load value at the address pointed by POINTER into NEXT.		
			CMP	MAX, NEXT						
			BHS	CNTU							;Subroutine. If MAX is higher or equal to NEXT, branch to CNTU.
												;BHS = Branch if Higher or Same. Checking condition flags C and Z being set after the CMP.
			MOV	MAX, NEXT						;If MAX is lower than NEXT, update MAX with the value in NEXT. Inside the "if" loop (BHS).
			CMP	MIN, NEXT
			BHS IF_MIN
			B	CNTU

IF_MIN		MOV	MIN, NEXT
		
CNTU		ADD	POINTER, POINTER, #4			;Modifies POINTER's value by adding 4 bytes, to reach the next value in the array.
			SUBS COUNT,	COUNT, #1				;COUNT = 5, 4, 3, 2, 1, 0
			BNE	AGAIN							;Continue if the value is =! 0 
			
STOP		B	STOP
			
			ENDP
			END