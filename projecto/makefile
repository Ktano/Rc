user: clear tcpcommandwriter.o commandlinereader.o fpt.o user.o folders
	gcc -g -o user tcpcommandwriter.o commandlinereader.o fpt.o user.o
tcpcommandwriter.o: tcpcommandwriter.c 
	gcc -g -Wall -pedantic -c tcpcommandwriter.c
commandlinereader.o: commandlinereader.c 
	gcc -g -Wall -pedantic -c commandlinereader.c
fpt.o: fpt.c
	gcc -g -Wall -pedantic -c fpt.c
user.o: user.c fpt.o commandlinereader.o tcpcommandwriter.o
	gcc -g -Wall -pedantic -c user.c 
folders:
	mkdir -p input_files
	mkdir -p output_files
clear:
	clear
clean:
	rm -f -d -r *.o user input_files output_files
zip:
	zip ../proj_6.zip *.c *.h makefile