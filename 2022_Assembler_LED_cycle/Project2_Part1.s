@ BBB built-in LED cycle-through program
@ Turms on on-board LEDs one after the other for 1 s

@ Uses R0-R3, R5-R9

@ Dmitrii Fotin December 2022

.text
.global _start
_start:

	@initialize clock
	MOV R0,#0x02
	LDR R1,=0x44E000AC
	STR R0,[R1]
	
	@initialize GPIO1 LEDs as data out
	LDR R0,=0x4804C000
	ADD R1,R0,#0x134
	LDR R9,[R1]
	MOV R2,#0xFE1FFFFF
	AND R9,R2,R9
	STR R9,[R1]
	
	LDR R8,=0x00FFFFFF	@1s clock counter for 1GHz processor
	MOV R7,#0x01000000	@adress for LED3, R7
	MOV R6,#0x00200000	@adress for LED0, R6
	MOV R4,#1
	
	ADD R3,R0,#0x194
	MOV R5,R7

	@turn on LED loop
LED_LOOP:
	STR R5,[R3]
	B WAIT_1S

	@delay loop of 1s
WAIT_1S:
	SUBS R8,R8,#1
	BNE WAIT_1S
	SUB R3,R3,#0x4
	STR	R5,[R3]
	ADD R3,R3,#0x4
	LDR R8,=0x000FFFFF
	TEQ R4,#1
	BEQ STEP_FOR_LED
	BNE STEP_BACK_LED

	@step to next LED (forward direction)
STEP_FOR_LED:
	MOVS R5, R5, LSR #1
	TEQ R5,#0x00200000
	BEQ BACKWARD
	B LED_LOOP

	@step to next LED (backward direction)
STEP_BACK_LED:
	MOVS R5, R5, LSL #1
	TEQ R5,#0x01000000
	BEQ FORWARD
	B LED_LOOP

	@set direction for cycling through LEDs
FORWARD:
	ADD R4,R4,#1
	B LED_LOOP
BACKWARD:
	SUB R4,R4,#1
	B LED_LOOP

.END