#include "fpt.h"
#include "string.h"
char* taskDescription(char* PTC)
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


