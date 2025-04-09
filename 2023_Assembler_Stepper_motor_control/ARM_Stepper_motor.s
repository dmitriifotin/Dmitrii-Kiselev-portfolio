@ BBB communicationg through I2C with motor driver
@ Transmits a series of steps to run a stepper motor 360 degrees
@ Every time a button is pressed

@ Uses R0-R12

@ Dmitrii Fotin March 2023

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
	
	MOV R7,#5000		@delay loop counter for setup transmissions
	MOV R10,#60000		@delay loop counter between each step transmission
	LDR R8, =PWM_Adresses	@load address array pointer
	LDR R9, =PWM_Words		@load word array pointer
	LDRH R4, [R8]		@load first address to R4
	LDRH R5, [R9]		@load first word to R4
	MOV R11,#800		@50 steps * 16 transmits per step
	MOV R6,#16			@16 transmits
	MOV R12,#0 			@Status bool value (motor cycle vs wait loop)

	@initialize clock
	LDR R0,=0x44E00044
	MOV R1,#0x00000002
	STR R1,[R0]

	@initialize I2C pins
	LDR R0,=0x44E10954
	MOV R1,#0x00000032
	STR R1,[R0]
	
	LDR R0,=0x44E10950
	MOV R1,#0x00000032
	STR R1,[R0]
	
	@sleep
	LDR R0,=0x4819C010
	MOV R1,#0x00000002
	STR R1,[R0]
	
	@prescaler
	LDR R0,=0x4819C0B0
	MOV R1,#0x00000003
	STR R1,[R0]
	
	@SCLL
	LDR R0,=0x4819C0B4
	MOV R1,#0x00000008
	STR R1,[R0]
	
	@SCLH
	LDR R0,=0x4819C0B8
	MOV R1,#0x0000000A
	STR R1,[R0]
	
	@OA
	LDR R0,=0x4819C0A8
	MOV R1,#0x00000000
	STR R1,[R0]
	
	@I2C CON
	LDR R0,=0x4819C0A4
	MOV R1,#0x00008600
	STR R1,[R0]	

	@slave address
	LDR R0,=0x4819C0AC
	MOV R1,#0x00000070
	STR R1,[R0]
	
	@I2C_CNT
	LDR R0,=0x4819C098
	MOV R1,#0x00000002
	STR R1,[R0]	
	
	@Initial PCA setup
	
	@MODE 1
	BL I2C_CON
	BL POLL_BB
	BL I2C_CON_START_STOP
	BL POLL_XRDY
	
	MOV R4,#0x00000000
	MOV R5,#0x00000011
	LDR R0,=0x4819C09C
	STR R4,[R0]
	STR R5,[R0]
	BL DELAY
	
	@PRESCALE
	BL I2C_CON
	BL POLL_BB
	BL I2C_CON_START_STOP
	BL POLL_XRDY
	
	MOV R4,#0x000000FE
	MOV R5,#0x00000003
	LDR R0,=0x4819C09C
	STR R4,[R0]
	STR R5,[R0]
	BL DELAY
	
	@MODE 1
	BL I2C_CON
	BL POLL_BB
	BL I2C_CON_START_STOP
	BL POLL_XRDY
	
	MOV R4,#0x00000000
	MOV R5,#0x00000081
	LDR R0,=0x4819C09C
	STR R4,[R0]
	STR R5,[R0]
	BL DELAY
		
	@MODE 2
	BL I2C_CON
	BL POLL_BB
	BL I2C_CON_START_STOP
	BL POLL_XRDY
	
	MOV R4,#0x00000001
	MOV R5,#0x00000004
	LDR R0,=0x4819C09C
	STR R4,[R0]
	STR R5,[R0]
	BL DELAY
	
	@initialize clock
	LDR R0,=0x44E000AC
	LDR R1,=0x00040002
	STR R1,[R0]
	
	@Enable debounce on GPIO1_8 for 31 us
	LDR R0,=0x44E00150
	MOV R1,#0x00000008
	STR R1,[R0]
	LDR R0,=0x44E00154
	MOV R1,#0xA0
	STR R1,[R0]
	
	@set up the button input GPIO

	LDR R0,=0x4804C14C	@GPIO1 base address + falling edge offset assignment to R0
	MOV R1,#0x00000008	@GPIO_3 address assignment to R1
	LDR R2,[R0]			@load GPIO1 base address + falling edge offset to R0
	ORR R2,R2,R1		@zero out the bit for GPIO_3 and store the final word in R0
	STR R2,[R0]			@store the address of the final word in R0
	ADD R1,R0,#0x34		@GPIO1 base address + OE assignment to R0
	STR R2,[R1]			@enable GPIO_3 request to PONTRPEND1

	@initialize INTC
	LDR R0,=0x482000E8	@address of INTC_MIR_CLEAR3
	MOV R1,#0x04		@value to unmask INTC INT 98, GPIOINT1A
	STR R1,[R0]			@write to INTC_MIR_CLEAR3
	
	@clear interrupt
	LDR R0,=0x48200048
	MOV R1,#0x01
	STR R1,[R0]
	
	MRS R3, CPSR		@copy CPR to R3
	BIC R3,#0x80		@clear bit 7
	MSR CPSR_c,R3		@write back to CPSR
	
	B WAIT_FOR_INTERRUPT

@Delay loop for PCA setups
DELAY:
	SUBS R7,R7,#1
	BNE DELAY
	MOV R7,#5000
	MOV PC, LR

@Delay loop between each motor step transmission
DELAY2:
	SUBS R10,R10,#1
	BNE DELAY2
	MOV R10,#60000
	MOV PC, LR

@the following 4 functions are called each time anything is to be transmitted to PCA
I2C_CON:
	@I2C CON - reset
	
	LDR R0,=0x4819C0A4
	MOV R1,#0x00008600
	STR R1,[R0]
	MOV PC, LR

POLL_BB:
	@poll BB - check if I2C bus is available for transmission
	LDR R0, =0x4819C024
	LDR R1, [R0]
	MOV R2, #0x00001000
	AND R2,R2,R1
	TEQ R2, #0x00000000
	BNE POLL_BB
	MOV PC, LR
	
I2C_CON_START_STOP:
	@I2C CON - set START/STOP for CON register
	LDR R0,=0x4819C0A4
	MOV R1,#0x00008603
	STR R1,[R0]
	MOV PC, LR
	
POLL_XRDY:
	@check if PCA is ready to receive
	LDR R0, =0x4819C024
	LDR R1, [R0]
	MOV R2, #0x10
	AND R2,R2,R1
	TEQ R2, #0x10
	BNE POLL_XRDY
	MOV PC, LR

	@empty loop that waits for button interrupt, if status bool R12 is 1, go to motor loop
WAIT_FOR_INTERRUPT:
	NOP
	TEQ R12,#0
	BNE SETUP
	B WAIT_FOR_INTERRUPT

	@chaining the interrupt vector with custom instructions
INT_DIRECTOR:
	STMFD SP!,{R0-R3,LR}		@push registers on stack
	LDR R0,=0x482000F8			@address of INTC-PENDING-IRQ3
	LDR R1,[R0]					@read INTC-PENDING-IRQ3
	TEQ R1,#0x00000004			@check bit 2
	BNE WAIT_FOR_INTERRUPT		@if not from GPIOINT1A, go to wait loop
	LDR R0,=0x4804C02C			@if yes, load GPIO1_IRQSTATUS_0 address
	LDR R1,[R0]					@read STATUS
	TEQ R1,#0x00000008			@check if bit 3 == 1
	BEQ INT_CLEAR				@if yes, go to check/set bool for LED loop vs. empty loop
	LDR R0,=0x48200048			@address of INTC_CONTROL
	MOV R1,#0x01				@clear bit 0
	STR R1,[R0]					@write
	LDMFD SP!,{R0-R3,LR}		@restore registers
	SUBS PC,LR,#4				@return to PC address

INT_CLEAR:
	MOV R1,#0x00000008			@turn off GPIO_3 interrupt
	STR R1,[R0]					@write to GPIO1_IRQSTATUS_0
	
	ADD R12,#1 					@Set status to 1 (to run motor loop)
		
	LDR R0,=0x48200048			@address of INTC_CONTROL
	MOV R1,#0x01				@clear bit 0
	STR R1,[R0]					@write
	LDMFD SP!,{R0-R3,LR}		@restore registers
	SUBS PC,LR,#4				@return to PC address

SETUP:
	@close the main OFF switch
	BL I2C_CON
	BL POLL_BB
	BL I2C_CON_START_STOP
	BL POLL_XRDY
	LDR R0,=0x4819C09C
	MOV R4,#0x000000FD
	MOV R5,#0x00000000
	STR R4,[R0]
	STR R5,[R0]
	BL DELAY2
	
	BL I2C_CON
	BL POLL_BB
	BL I2C_CON_START_STOP
	BL POLL_XRDY	
	@set PWMA to 1
	LDR R0,=0x4819C09C
	MOV R4,#0x0000000F
	MOV R5,#0x00000010
	STR R4,[R0]
	STR R5,[R0]
	BL DELAY2
	
	BL I2C_CON
	BL POLL_BB
	BL I2C_CON_START_STOP
	BL POLL_XRDY	
	@set PWMB to 1
	LDR R0,=0x4819C09C
	MOV R4,#0x00000023
	MOV R5,#0x00000010
	STR R4,[R0]
	STR R5,[R0]
	BL DELAY2
	
TRANSMIT:

	@Load current PCA pin address and the word to send to it (2 bytes)
	LDRH R4,[R8]
	LDRH R5,[R9]
	
	BL I2C_CON
	BL POLL_BB
	BL I2C_CON_START_STOP
	BL POLL_XRDY
	
	@Transmit both bytes to PCA
	LDR R0,=0x4819C09C
	STR R4,[R0]
	STR R5,[R0]
	BL DELAY2
	
	SUB R6,R6,#1 			@decrement the address counter
	SUBS R11,R11,#1 		@decrement the instruction counter
	BEQ END 				@if instruction counter is 0, i.e. motor's run 360 degrees, go to END branch
	TEQ R4,#0x1F 			@if the last address is encountered
	BEQ RESET_ADDRESS_ARRAY	@go to load the first address to R4 again
	ADD R8,R8,#2 			@else increment the address and the word array indices by 2 bytes each
	ADD R9,R9,#2

	B TRANSMIT 				@go back to the beginning of this branch to transmit the next index address and word
		
RESET_ADDRESS_ARRAY:
	LDR R8, =PWM_Adresses
	TEQ R6, #0x00 			@if the address counter is not 0, go to increment the word array index
	BNE INC_WORD_ARRAY		@else, reset the word array index
RESET_WORD_ARRAY:
	LDR R9, =PWM_Words
	MOV R6,#16 				@reset address counter
	B TRANSMIT
INC_WORD_ARRAY:
	ADD R9, R9, #2 			@increment the word array by 2 bytes
	B TRANSMIT
	
END:
	@reset all counters
	MOV R11,#800
	MOV R6,#16
	MOV R12,#0

	BL I2C_CON
	BL POLL_BB
	BL I2C_CON_START_STOP
	BL POLL_XRDY	
	@open the main OFF switch to turn off all PCA pins
	LDR R0,=0x4819C09C
	MOV R4,#0x000000FD
	MOV R5,#0x00000001
	STR R4,[R0]
	STR R5,[R0]
	BL DELAY2
	
	@reload array pointers
	LDR R8,=PWM_Adresses
	LDR R9,=PWM_Words
	
	@clear interrupt
	LDR R0,=0x48200048
	MOV R1,#0x01
	STR R1,[R0]
	
	@go back to wait loop until next button press
	B WAIT_FOR_INTERRUPT
	
.data
PWM_Adresses: .HWORD 0x17, 0x15, 0x1B, 0x1F
PWM_Words: .HWORD 0x10, 0x00, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00
.align 2
STACK1:	.rept 1024
		.word 0x0000
		.endr
STACK2:	.rept 1024
		.word 0x0000
		.endr

.END