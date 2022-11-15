all: simulador monitor

clean: 
		rm -f "monitor.o" "simulador.o" "simulador" "monitor"

simulador: simulador.c header.h 
		gcc -c -g simulador.c && gcc -g -o simulador simulador.o -lpthread

monitor: monitor.c header.h
		gcc -c -g monitor.c && gcc -g -o monitor monitor.o -lpthread
