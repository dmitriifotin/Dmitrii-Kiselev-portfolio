@ BBB built-in LED cycle-through program with button input
@ Turns on on-board LEDs one after the other for 1s each if button is pressed
@ Turns off all LEDs if button pressed again
@ COntinues to cycle through LEDs or do nothing with each button press

@ Uses R0-R10, R12

@ Dmitrii Fotin December 2022

.text
.global _start
.global INT_DIRECTOR
_start:
	@stack initialization
	LDR R13,=STACK1
	ADD R13,R13,#0x1000
	CPS #0x12
	LDR R13,=STACK2
	ADD R13,R13,#0x1000
	CPS #0x13

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
	
	MOV R12,#0			@boolean register that determines if program runs the LED loop or the empty loop
	
	@set up the button input GPIO
	ADD R6,R0,#0x14C	@GPIO1 base address + falling edge offset assignment to R6
	MOV R7,#0x00000008	@GPIO_3 address assignment to R7
	LDR R10,[R6]		@load GPIO1 base address + falling edge offset to R10
	ORR R10,R10,R7		@zero out the bit for GPIO_3 and store the final word in R10
	STR R10,[R6]		@store the address of the final word in R6
	ADD R6,R0,#0x34		@GPIO1 base address + OE assignment to R6
	STR R7,[R6]			@enable GPIO_3 request to PONTRPEND1

	@initialize INTC
	LDR R6,=0x482000E8	@address of INTC_MIR_CLEAR3
	MOV R7,#0x04		@value to unmask INTC INT 98, GPIOINT1A
	STR R7,[R6]			@write to INTC_MIR_CLEAR3
	
	MRS R10, CPSR		@copy CPR to R10
	BIC R10,#0x80		@clear bit 7
	MSR CPSR_c,R10		@write back to CPSR
	
	@default values for program start & when everything needs to be reset before going back into empty loop after LED loop
RESET:	
	LDR R8,=0x000FFFFF	@1s clock counter for 1GHz processor
	ADD R3,R0,#0x190	@default GPIO1 address + clear values offset
	MOV R5,#0x01E00000	@adress for all on-board LEDs
	STR R5,[R3]			@clear all LEDs
	ADD R3,R3,#0x4		@offset to set LEDs
	MOV R4,#1			@boolean value that determines if LEDs cycle forward or backward
	MOV R5,#0x01000000	@address for on-board LED3
	
	@empty loop that waits for button interrupt
WAIT_FOR_BUTTON:
	NOP
	TEQ R12,#1			@as the interrupt returns to the empty loop after each even button press due to PC value,
	BEQ LED_LOOP		@R12 is changed during interrupt to allow to branch to LED loop
	BNE WAIT_FOR_BUTTON	@otherwise, continue executing empty loop

	@chaining the interrupt vector with custom instructions
INT_DIRECTOR:
	STMFD SP!,{R0,R6,R7,R10,LR}	@push registers on stack
	LDR R0,=0x482000F8			@address of INTC-PENDING-IRQ3
	LDR R6,[R0]					@read INTC-PENDING-IRQ3
	TEQ R6,#0x00000004			@check bit 2
	BNE WAIT_FOR_BUTTON			@if not from GPIOINT1A, go to empty loop
	LDR R0,=0x4804C02C			@if yes, load GPIO1_IRQSTATUS_0 address
	LDR R6,[R0]					@read STATUS
	TEQ R6,#0x00000008			@check if bit 3 == 1
	BEQ ON_OR_OFF				@if yes, go to check/set bool for LED loop vs. empty loop
	LDMFD SP!,{R0,R6,R7,R10,LR}	@if not, then not button interrupt, restore registers
	B WAIT_FOR_BUTTON			@and go back to empty loop

ON_OR_OFF:
	MOV R6,#0x00000008			@turn off GPIO_3 interrupt
	STR R6,[R0]					@write to GPIO1_IRQSTATUS_0
	LDR R0,=0x48200048			@address of INTC_CONTROL
	MOV R6,#0x01				@clear bit 0
	STR R6,[R0]					@write to INTC_CONTROL
	
	TEQ R12,#1					@check if R12 (bool for LED vs. empty) is 1/0
	BEQ ON						@go to ON branch to run the LED loop
	BNE OFF						@go to OFF branch to run the empty loop
	
	@if the last state was LED loop on, change R12 to cycle through empty loop next
	@restore registers and go back to PC address (in the LED loop)
ON:
	SUB R12,R12,#1
	LDMFD SP!,{R0,R6,R7,R10,LR}
	SUBS PC,LR,#4

	@if the last state was empty loop on, change R12 to cycle through LED loop next
	@restore registers and go back to PC address (in the empty loop)
OFF:
	ADD R12,R12,#1
	LDMFD SP!,{R0,R6,R7,R10,LR}
	SUBS PC,LR,#4
	

	@turn on LED loop, turns on the LED at the current address in R5
	@R3 contains the based GPIO1 address + set value offset
LED_LOOP:
	STR R5,[R3]
	B WAIT_1S

	@delay loop that also includes steps to turn off the LED currently on after a delay
WAIT_1S:
	TEQ R12,#1		@PC returns to this point after interrups
	BNE RESET		@if R12 is set to 0, go to empty loop
	SUBS R8,R8,#1		@decrement time counter, i.e. delay until counter is 0
	BNE WAIT_1S
	SUB R3,R3,#0x4		@offset to clear LED
	STR R5,[R3]		@clear LED
	ADD R3,R3,#0x4		@offset back to set LED
	LDR R8,=0x000FFFFF	@reset counter
	TEQ R4,#1		@check cycle direction (1 forward, 0 backward)
	BEQ STEP_FOR_LED	@go to forward branch if R4 is 1
	BNE STEP_BACK_LED	@go to backward branch if R4 is 0

	@step to next LED (forward direction)
STEP_FOR_LED:
	MOVS R5, R5, LSR #1	@divide LED address by 2 (step to lower bit)
	TEQ R5,#0x00200000	@check if LED0/lowest bit reached
	BEQ BACKWARD		@branch to change direction if yes
	B LED_LOOP		@restart LED loop if no

	@step to next LED (backward direction)
STEP_BACK_LED:
	MOVS R5, R5, LSL #1	@multiply LED address by 2 (to to higher bit)
	TEQ R5,#0x01000000	@check if LED3/highest bit is reached
	BEQ FORWARD		@branch to change direction if yes
	B LED_LOOP		@restart LED loop if no

	@set direction for cycling through LEDs
FORWARD:
	ADD R4,R4,#1		@set R4 to 1 to go in forward to direction
	B LED_LOOP		@restart LED loop
BACKWARD:
	SUB R4,R4,#1		@set R4 to 0 to go in backward to direction
	B LED_LOOP		@restart LED loop

.data
.align 2
STACK1:	.rept 1024
		.word 0x0000
		.endr
STACK2:	.rept 1024
		.word 0x0000
		.endr

.END