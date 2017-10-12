#include "fpt.h"
#include "string.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>

#define BUFFER_MAX 1024

char *taskDescription(char *PTC)
{
  if (strcmp(PTC, PTC_WORDCOUNT) == 0)
    return DSC_WORDCOUNT;
  if (strcmp(PTC, PTC_LONGESTWORD) == 0)
    return DSC_LONGESTWORD;
  if (strcmp(PTC, PTC_UPPER) == 0)
    return DSC_UPPER;
  if (strcmp(PTC, PTC_LOWER) == 0)
    return DSC_LOWER;
  return "";
}

/** returns the number of words in a file
 * each word is split by a non alphanumeric character
 */

int fileWordCount(char *filename)
{
  int total = 0;
  FILE *fd;
  char c, p;
  if ((fd = fopen(filename, "r")) == NULL)
  {
    printf("ERROR: %s", strerror(errno));
    return -1;
  }
  p = fgetc(fd);
  if (isalnum(p))
    total++;

  while (feof(fd) == 0)
  {
    c = fgetc(fd);
    if (isalnum(p) == 0 && isalnum(c) != 0)
      total++;
    p = c;
  }
  fclose(fd);
  return total;
}

/**Function that agregates the result of multiple word counts
 */

int agregateWordCount(int *results, int length)
{
  int total, i;

  for (i = 0; i < length; i++)
    total += results[i];
  return total;
}

/**function to find the longest work in a file
 * returns the number of chars in the longest word
 * and the longest word will be in the buffer
*/
int fileLongestWord(char *filename, char *buffer, int buffersize)
{
  int maxtotal = 0, currentTotal = 0;
  FILE *fd;
  char currentLongest[BUFFER_MAX], c;
  if ((fd = fopen(filename, "r")) == NULL)
  {
    printf("ERROR: %s", strerror(errno));
    return -1;
  }

  while (feof(fd) == 0)
  {
    c = fgetc(fd);
    if (isalnum(c) && currentTotal < buffersize)
    {
      currentLongest[currentTotal] = c;
      currentTotal++;
    }
    else if (isalnum(c) == 0)
    {
      if (currentTotal > maxtotal)
        maxtotal = currentTotal;
      currentLongest[currentTotal] = '\0';
      strcpy(buffer, currentLongest);
    }
    else
    {
      printf("error: Longest word is longer than buffer");
      fclose(fd);
      return -1;
    }
  }
  fclose(fd);
  return maxtotal;
}

/* Function to find the result of multiple find longest words*/

int agregateLongestWord(char **words, int length, char *buffer, char buffersize)
{
  int total = 0, i, len;

  for (i = 0; i < length; i++)
  {
    len = strlen(words[i]);
    if (len > total && len < buffersize)
    {
      total = len;
      strcpy(buffer, words[i]);
    }
    else
      return -1;
  }
  return total;
}


/** receives two file names one for origin one for destine
 * and a function point on what to do with each char
 **/

int changeAllChars(char *fileorg, char *filedst, int (*f)(int))
{
  int total = 0;

  FILE *fdOrg, *fdDst;
  char c;
  if ((fdOrg = fopen(fileorg, "r")) == NULL)
  {
    printf("ERROR: %s", strerror(errno));
    return -1;
  }

  if ((fdDst = fopen(filedst, "w")) == NULL)
  {
    printf("ERROR: %s", strerror(errno));
    return -1;
  }

  while (feof(fdOrg) == 0)
  {
    c = fgetc(fdOrg);
    c = fputc(f(c), fdDst);
    total++;
  }
  return total;
}

int agregateChangeAllChar(char **OrigFiles,int length, char *DestFile){
  int total = 0,i;
  FILE *fdOrg, *fdDst;
  char c;
  if ((fdDst = fopen(DestFile, "w")) == NULL)
  {
    printf("ERROR: %s", strerror(errno));
    return -1;
  }

  for (i = 0; i < length; i++)
  {
    if ((fdOrg = fopen(OrigFiles[i], "r")) == NULL)
    {
      printf("ERROR: %s", strerror(errno));
      fclose(fdDst);
      return -1;
    }

    while (feof(fdOrg) == 0)
    {
      c = fgetc(fdOrg);
      c = fputc(c, fdDst);
      total++;
    }
    fclose(fdOrg);
  }

  fclose(fdDst);
  return total;
}
