;At the end, while compiling, I'd be able to open the Memory Window, search for the address 0x20000000 (where the SRAM begins),
;set "Integer, Decimal" and check our first three values: 5, 4, and 200.
		AREA	myCode, CODE, READONLY
		ENTRY
		EXPORT __main

__main
		LDR		R0, =A ;Load Register. We start by pointing the address of R0 to the one of A.
		MOV		R1, #5 ;Move. R1's value is 5.
		STR		R1, [R0];Store Register. R0 is the memory address destination, where we will store R1's value.
		;The use of square brackets around a register denote that the contents of that register should be interpreted as a memory address.
		;[] indicated indirect addressing.
		
		;We can now reuse the registers since we initialized in memory already.
		
		LDR		R0, =D
		MOV		R1, #4
		STR		R1, [R0]
		
		LDR		R0, =C
		MOV		R1, #200
		STR		R1, [R0]

STOP	B		STOP
		


		AREA	myData, DATA, READWRITE
		;Allocate the following in the SRAM memory.	
			
A		SPACE	4 ; (in bytes)
D		SPACE	4
C		SPACE	4
	
		END
			
