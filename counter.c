
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int FTPcounter()
{
  char* inFileName = "file_processing_tasks.txt";
  char* outFileName = "tmp.txt";
  char* linetorem = "WCT 192.168.1.2 5800\n";
  FILE* inFile = fopen(inFileName, "r");
  FILE* outFile = fopen(outFileName, "w+");
  char line [1024]; // maybe you have to user better value here

  if( inFile == NULL )
  {
      printf("Open Error");
  }

  while( fgets(line, sizeof(line), inFile) != NULL )
  {
      if(strcmp(line, linetorem) != 0)
      {
          fprintf(outFile, "%s", line);
      }
  }


  fclose(inFile);
  fclose(outFile);

  // possible you have to remove old file here before
  if( !rename(inFileName, outFileName) )
  {
      printf("Rename Error");
  }
}
