#ifndef FPT_H
#define FPT_H

#define PTC_WORDCOUNT "WCT"
#define PTC_LONGESTWORD "FLW"
#define PTC_UPPER "UPP"
#define PTC_LOWER "LOW"

#define RT_WORDCOUNT "R"
#define RT_LONGESTWORD "R"
#define RT_UPPER "F"
#define RT_LOWER "F"

#define DSC_WORDCOUNT "Word count"
#define DSC_LONGESTWORD "Find longest word"
#define DSC_UPPER "Convert text to upper case"
#define DSC_LOWER "Convert text to lower case"

char* taskDescription(char* PTC);
int fileWordCount(char *filename);
int agregateWordCount(int *results, int length);
int fileLongestWord(char *filename,char *buffer, int buffersize);
int agregateLongestWord(char **words,int lenght,char *buffer, char buffersize);
int changeAllChars(char *fileorg, char *filedst, int (*f)(int));

#endif
