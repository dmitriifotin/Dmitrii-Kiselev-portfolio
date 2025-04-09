all: timer.o alarmSig.o

clean:
	rm -f timer alarmSig

timer.o: timer.c 
	gcc -std=c99 -o timer timer.c 

alarmSig.o: alarmSig.c 
	gcc -std=c99 -o alarmSig alarmSig.c 
