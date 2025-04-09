Simon Says handheld game that shows a sequence of buttons lighting up and prompting the player to repeat it

The challenge was to utilize the smallest MCU (Adafruit Trinket) to introduce the concept of designing with limitations. Given the small number of pins, the design included a decoder to operate multiple LEDs and variable resistors to detect an analog signal from button inputs.
The code design had to account for three difficulty levels with various sequence play speeds and max lengths. Each difficulty also had a corresponding number of sequences, 3 at easy and 18 at hard, all of which had to be repeated correctly in order to win. The design had to reuse a lot of functionality over and over, so using functions and passing values around was critical in simplifying the script.

MCU: Adafruit Trinket
Script: Python
Components: Piezo, buttons, decoder (SN74HC138N IC)
Timeline: 1 month
Team role: circuit design (collab), code (overall architecture excluding LED and Piezo functions)