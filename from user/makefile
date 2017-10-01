user: clear tcpcommandwriter.o commandlinereader.o fpt.o user.o
	gcc -g -o user tcpcommandwriter.o commandlinereader.o fpt.o user.o
	mkdir input_files
	mkdir output_files
tcpcommandwriter.o: tcpcommandwriter.c 
	gcc -g -Wall -pedantic -c tcpcommandwriter.c
commandlinereader.o: commandlinereader.c 
	gcc -g -Wall -pedantic -c commandlinereader.c
fpt.o: fpt.c
	gcc -g -Wall -pedantic -c fpt.c
user.o: user.c fpt.o commandlinereader.o tcpcommandwriter.o
	gcc -g -Wall -pedantic -c user.c 
clear:
	clear
clean:
	rm -f -d -r *.o user input_files output_files
zip:
	zip ../proj_6.zip *.c *.h makefile