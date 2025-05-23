
RAM1_ADDR		EQU		0x20000000 ;Beginning of the RAM
RAM2_ADDR		EQU		0x20000100
;Addresses


					AREA	myCode, CODE, readonly
					ENTRY
					EXPORT 	__main
						
__main
					BL		FILL 		;Subroutine name
					BL		COPY 		;Subroutine 2 name
					
STOP				B		STOP
					
	
	
FILL				LDR		R1, =RAM1_ADDR	;Instruction to load from memory into a register.
					MOV		R0, #10
					LDR		R2, =0xDEADBEEF
					
L1					STR		R2, [R1]	;Loop 1
					ADD		R1, R1, #4
					SUBS	R0, R0, #1 	;Decrement the counter
					BNE		L1
					BX		LR 			;Finish the subroutine, return to the flow.
;Places the value 0xDEADBEEF 10 time into RAM1 address, by incrementing 4 places its address after every fill.
;First in 0x20000000, second in 0x20000004, third in 0x20000008,...

COPY				LDR		R1, =RAM1_ADDR ;Origin address
					LDR		R2, =RAM2_ADDR ;Destination address
					MOV		R0, #10
					
L2					LDR		R3, [R1] ;Loop 2
					STR		R3, [R2]
					ADD		R1, R1, #4
					ADD		R2, R2, #4
					SUBS	R0, R0, #1
					BNE		L2
					BX		LR 			;Exit from the subroutine
					
					
					END
					
					