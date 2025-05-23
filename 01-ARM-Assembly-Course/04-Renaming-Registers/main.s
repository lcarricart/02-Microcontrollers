
VAL1	RN		R1 ;Whenever I wanna call R1, I can use VAL1
NUM		EQU		0x344 ;This diretive doesn't work for registers, but serves the same purpose for other types.
VAL2	RN		R2
SUM		RN		R3

		
		AREA	rn,CODE,READONLY
		ENTRY
		EXPORT 	__main
__main
		MOV		VAL1, #0x25
		MOV		VAL2, #0x34
		
		ADD		SUM,VAL1,VAL2
		
STOP	B		STOP
		END