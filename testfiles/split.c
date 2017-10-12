#include <stdio.h>
#include <string.h>

int main() {

	FILE *sourcefile;
	FILE *partitionfile;

	char line[128], partition[9];
	int files=1, counter=1;
  char *file = "00001.txt";
  int servers = 2;
	int ch=0, lines=1;

	sourcefile = fopen(file,"r");


	while(!feof(sourcefile)){
	  ch = fgetc(sourcefile);
	  if(ch == '\n')
	    lines++;
	}
	rewind(sourcefile);

	printf("LINES: %d\n", lines);

  sprintf(partition, "%s00%d.txt", file, files);
	partitionfile = fopen(partition, "w");

	while (fgets(line, sizeof line, sourcefile)!=NULL) {
		if ((lines / servers)*files < counter &&files!=servers){
			fclose(partitionfile);

			files++;
			sprintf(partition, "%s00%d.txt", file, files);
			partitionfile = fopen(partition, "w");
		}
		counter++;
		fprintf(partitionfile,"%s\n", line);
	}
	fclose(sourcefile);
	return 0;
}
