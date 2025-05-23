
				AREA		myCode, CODE, READONLY
				ENTRY
				EXPORT		__main
					
__main
				LDR			R0, =0xF62562FA
				LDR			R1, =0xF412963B
				
				MOV			R2, #0x35
				MOV			R3, #0x21
				
				ADDS		R5, R2, R3
				;Now we wanna use the carry bit from this first computation, and to do so we use the ADC instruction
				ADC			R6, R2, R3		;R6 = R2 + R3 + Carry (1 or 0)
				
STOP			B			STOP

				END
				
