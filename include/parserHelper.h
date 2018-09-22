#ifndef _PARSE_HELP_
#define _PARSE_HELP_

#include "VCardParser.h"

VCardErrorCode fileCheck(char *fileName, FILE *fp);
char* upperCaseStr(char *a);
char* unfold(FILE *fp);
VCardErrorCode parseFile(char *buffer, Card **newCardObject);
int numSemiColons(char *a);
char *getGroup(char *token);
char *getProp(char *token);
char *getParam(char *token);
char *getValue(char *token);
#endif