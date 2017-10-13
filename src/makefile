user: clear tcpcommandwriter.o commandlinereader.o fpt.o user.o WS.o cs.o folders
	gcc -g -o user tcpcommandwriter.o commandlinereader.o fpt.o user.o
	gcc -g -o WS tcpcommandwriter.o commandlinereader.o fpt.o WS.o
	gcc -g -o CS tcpcommandwriter.o commandlinereader.o fpt.o cs.o
tcpcommandwriter.o: tcpcommandwriter.c
	gcc -g -Wall -pedantic -c tcpcommandwriter.c
commandlinereader.o: commandlinereader.c
	gcc -g -Wall -pedantic -c commandlinereader.c
fpt.o: fpt.c
	gcc -g -Wall -pedantic -c fpt.c
user.o: user.c fpt.o commandlinereader.o tcpcommandwriter.o
	gcc -g -Wall -pedantic -c user.c
cs.o: cs.c fpt.o tcpcommandwriter.o
	gcc -g -Wall -pedantic -c cs.c
WS.o: WS.c
	gcc -g -Wall -pedantic -c WS.c
folders:
	mkdir -p input_files
	mkdir -p output_files
clear:
	clear
clean:
	rm -f -d -r *.o user input_files output_files WS cs
zip:
	zip ../proj_6.zip *.c *.h makefile
testuser:
	./user -n tejo.tecnico.ulisboa.pt -p 58011 
testws:
	./WS WCT -n tejo.tecnico.ulisboa.pt -e 58011 -p 59000
