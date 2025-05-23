; For this project we want to avoid using the startup file, since it contains plenty of information that we're not eager to use for this project.
; This means we won't have the stack or our Reset Handler, and therefore we will add it to our main.s 

; Dont focus on everything, we are just getting started.

Stack		EQU		0x00000100		;Define start size of 256 bytes
	
			AREA	STACK, NOINIT, READWRITE, ALIGN = 3
	
StackMem 	SPACE	Stack
	
			AREA 	RESET, DATA, READONLY
			EXPORT	__Vectors
	
__Vectors
			DCD StackMem + Stack
			DCD Reset_Handler
			ALIGN
			
			AREA SimpleProject, CODE, READONLY, ALIGN = 2
			ENTRY
			EXPORT Reset_Handler
	
Reset_Handler
			MOV R5, #0x1234
			MOV R3, #0x1234
		
			ADD R6, R5, R3
		
STOP		B		STOP

	
			END
				
; After running the debugger, we can see how we were able to start a project without the startup file.