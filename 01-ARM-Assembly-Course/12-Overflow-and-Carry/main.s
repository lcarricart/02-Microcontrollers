

				AREA		myCode, CODE, READONLY
				ENTRY
				EXPORT		__main
					
__main
				LDR			R1,  =1000000000
				LDR			R2,  =2000000000
				LDR			R3,  =3000000000
				LDR			R4,  =4000000000	
				LDR			R5,  =4100000000
				
				MOV			R8, #0		;Initialization
				MOV			R9, #0		;Starts at 0 and accumulates the Carry bits
				
				ADDS		R8, R8, R1	;0 + R1 = R1
				ADC			R9, R9, #0	;R9 = R9 + 0 + Carry
				;The ADC instruction allows to make immediate use of the Carry bit as a result of the above instruction.
				ADDS		R8, R8, R2
				ADC			R9, R9, #0
				ADDS		R8, R8, R3
				ADC			R9, R9, #0
				ADDS		R8, R8, R4
				ADC			R9, R9, #0
				ADDS		R8, R8, R5
				ADC			R9, R9, #0
				
				;R9 will start storing the carries (C bit) when the addition start surpassing 2^32.
				
STOP			B			STOP

				END
				