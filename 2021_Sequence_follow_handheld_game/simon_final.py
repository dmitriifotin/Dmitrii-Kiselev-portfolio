import time
import board
import pulseio
import random
from analogio import AnalogIn
from digitalio import DigitalInOut, Direction

MSB = DigitalInOut(board.D0)
MID = DigitalInOut(board.D3)
LSB = DigitalInOut(board.D4)
MSB.direction = Direction.OUTPUT
MID.direction = Direction.OUTPUT
LSB.direction = Direction.OUTPUT

speaker = pulseio.PWMOut(board.A1, duty_cycle=0, variable_frequency=True)
analog_in = AnalogIn(board.A0)

LEDs = [ 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0]
tones = [220, 330, 440, 550]
endgame_tones = [550, 440, 330, 220]
wingame_tones = [220, 440, 220, 550]
generated_sequence = []
player_sequence = []
input_range = [0, 1, 2, 3]
game = False
select_level = False
level = 4
timer = 0

def select_difficulty():
	timer = int(time.monotonic())
	i = 0
	while True:
    	if buttons_input() != 8:
        	return buttons_input()
        	break
    	MSB.value = LEDs[0 + i * 3]
    	MID.value = LEDs[1 + i * 3]
    	LSB.value = LEDs[2 + i * 3]
    	time.sleep(.2)
    	i += 1
    	if i == 4:
        	i = 0

def play_generated_sequence():
	next = random.randint(0, 3)
	generated_sequence.append(next)
	for i in generated_sequence:
    	MSB.value = LEDs[0 + i * 3]
    	MID.value = LEDs[1 + i * 3]
    	LSB.value = LEDs[2 + i * 3]
    	speaker.frequency = tones[i]
    	speaker.duty_cycle = 65535 // 2
    	time.sleep(0.3 - len(generated_sequence) * 0.01)
    	speaker.duty_cycle = 0
    	MSB.value, MID.value, LSB.value = 0, 0, 0
    	time.sleep(0.3 - len(generated_sequence) * 0.01)

def buttons_input():
	global timer
	global select_level
	global game
	while True:
    
    	if 40000 <= analog_in.value < 44000:
        	return 5
    	if 44000 <= analog_in.value < 54000 and select_level:
        	return 0
    	if 54000 <= analog_in.value < 60000 and select_level:
        	return 1
    	if 60000 <= analog_in.value < 63000 and select_level:
        	return 2
    	if 63000 <= analog_in.value < 66000 and select_level:
        	return 3
    	if (int(time.monotonic()) - timer) > (5 - len(generated_sequence) * 0.175) and game:
        	return 7
    	if ((int(time.monotonic()) - timer) > 10) and select_level:
        	return 7
    	if analog_in.value < 40000:
        	return 8

def play_input(input_char):
	MSB.value = LEDs[0 + input_char * 3]
	MID.value = LEDs[1 + input_char * 3]
	LSB.value = LEDs[2 + input_char * 3]
	speaker.frequency = tones[input_char]
	speaker.duty_cycle = 65535 // 2
	time.sleep(0.3)
	MSB.value, MID.value, LSB.value = 0, 0, 0
	speaker.duty_cycle = 0
	time.sleep(0.2)

def blink():
	for p in input_range:
    	MSB.value = LEDs[0 + p * 3]
    	MID.value = LEDs[1 + p * 3]
    	LSB.value = LEDs[2 + p * 3]
    	speaker.frequency = tones[p]
    	speaker.duty_cycle = 65535 // 2
    	time.sleep(0.1)
    	speaker.duty_cycle = 0
    	MSB.value, MID.value, LSB.value = 0, 0, 0
    	time.sleep(0.1)

def endgame():
	blink()
	global game
	global select_level
	game = False
	select_level = False
	player_sequence.clear()
	generated_sequence.clear()
	for i in input_range:
    	MSB.value = LEDs[9 - i * 3]
    	MID.value = LEDs[10 - i * 3]
    	LSB.value = LEDs[11 - i * 3]
    	speaker.frequency = endgame_tones[i]
    	speaker.duty_cycle = 65535 // 2
    	time.sleep(0.1)
    	MSB.value, MID.value, LSB.value = 0, 0, 0
    	speaker.duty_cycle = 0
    	time.sleep(0.1)

def victory():
	for p in input_range:
    	MSB.value = LEDs[9 - p * 3]
    	MID.value = LEDs[10 - p * 3]
    	LSB.value = LEDs[11 - p * 3]
    	speaker.frequency = wingame_tones[p]
    	speaker.duty_cycle = 65535 // 2
    	time.sleep(0.05)
    	speaker.duty_cycle = 0
    	time.sleep(0.05)
	MSB.value, MID.value, LSB.value = 0, 0, 0
	player_sequence.clear()
	generated_sequence.clear()
	global game
	game = False
	time.sleep(.2)

while True:
	if buttons_input() == 5:
    	blink()
    	select_level = True
    	time.sleep(.2)
    	timer = int(time.monotonic())
    	while select_level:
        	level = select_difficulty()
        	print(level)
       	 
        	if level in input_range:
            	play_input(level)
            	game = True
            	break
        	if level == 5 or level == 7:
            	endgame()
            	break

	while game:
    	i = 0
    	play_generated_sequence()
    	print(generated_sequence)
    	while len(generated_sequence) != len(player_sequence):
        	timer = int(time.monotonic())
        	input_char = buttons_input()
       	 
        	if input_char in input_range:
            	player_sequence.append(input_char)
            	play_input(input_char)
            	print(player_sequence)
       	 

        	if input_char != 8:
            	if (buttons_input() == 5 or input_char == 5 or
                    	buttons_input() == 7 or input_char == 7 or
                    	input_char != generated_sequence[i]):
                	endgame()
                	break
            	i += 1
            	if i + 1 > len(generated_sequence):
                	break
   	 
    	if not game and not select_level:
        	break
    	if len(player_sequence) == len(generated_sequence):
        	if len(player_sequence) == (level + 1) * 5:
            	victory()
            	break
        	else:
            	player_sequence = []
           	 
    	time.sleep(.1)
	time.sleep(.1)
time.sleep(.1)
