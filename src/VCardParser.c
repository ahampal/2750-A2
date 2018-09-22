#include "parserHelper.h"
#include <ctype.h>

#define DEBUG 1

char* unfold(FILE *fp) {
    char *buff;
    int fileLen;

    fseek(fp, 0, SEEK_END);
    fileLen = ftell(fp);
    rewind(fp);
    buff = malloc(sizeof(char)* (fileLen + 1));

    /*
    load into memory
    remove CRLF and shift array over. NEED TO CHECK FOR LINE LENGTH.
    */

    for(int i = 0; i < fileLen; i++) {
        buff[i] = fgetc(fp);
        if(i > 1) {
            if(buff[i] == ' ' && buff[i - 1] == '\n' && buff[i - 2] == '\r') {
                i = i - 3;
            }
        }
        if( i == fileLen - 1) {
            buff[i] = '\0';
        }
    }

    buff[fileLen] = '\0';

    return buff;
}

VCardErrorCode fileCheck(char *fileName, FILE *fp) {
    char *extension;
    int fileLength;

    //file name check
    if (fp == NULL) {
        if(DEBUG) {printf("Card could not be created. Cannot find %s.\n", fileName);}
        return INV_FILE;
    }

    //file extension check
    fileLength = strlen(fileName);
    extension = strrchr(fileName, '.');

    if(DEBUG) {printf("fileLen: %d\n", fileLength);}
    if(DEBUG) {printf("extension: %s\n", extension);}

    if(strcmp(extension, ".vcf") != 0 && strcmp(extension, ".vcard")!= 0 )  {
        if(DEBUG) {printf("Card could not be created. Improper file type.\n");}
        return INV_FILE;
    }

    return OK; 
}

VCardErrorCode parseFile(char *buffer, Card **newCardObject) {
    char *token;
    char *lGroup;
    char *lProp;
    char *lParam;
    char *lVal;
    Card *ptr;

    if(endBuff(buffer) != OK) {
        if(DEBUG) {printf("Card could not be created. Missing End.\n");}
        return INV_CARD;
    }
    
    ptr = *newCardObject;
    token = strtok(buffer, "\r\n");
    if(token == NULL) {
        return INV_CARD;
    }
    
    if(strcmp(upperCaseStr(token), "BEGIN:VCARD") != 0) {
        if(DEBUG) {printf("Card could not be created. Missing Begin.\n");}
        return INV_CARD;
    }

    token = strtok(NULL, "\r\n");
    if(token == NULL) {
        return INV_CARD;
    }

    if(strcmp(upperCaseStr(token), "VERSION:4.0") != 0) {
        if(DEBUG) {printf("Card could not be created. File must be version 4.0.\n");}
        return INV_CARD;
    }

    token = strtok(NULL, "\r\n");
    if(token == NULL) {
        return INV_CARD;
    }

    if(endBuff(buffer) != OK) {
        if(DEBUG) {printf("Card could not be created. Missing End.\n");}
        return INV_CARD;
    }

    while(token != NULL && strcmp(token, "END:VCARD") != 0) {
        lGroup = getGroup(token);
        lProp = getProp(token);
        lParam = getParam(token);
        lVal = getValue(token);
        if(lGroup != NULL) {
            if(DEBUG) {printf("\nGroup: %s\n", lGroup);}
            free(lGroup);
        }

        if(DEBUG) {printf("\nProperty: %s\n", lProp);}
        free(lProp);
        if(lParam != NULL) {
            if(DEBUG) {printf("Param: %s\n", lParam);}
            free(lParam);
        }
        if(DEBUG) {printf("Value: %s\n", lVal);}
        free(lVal);
        token = strtok(NULL, "\r\n");
    }
    return OK;
}

VCardErrorCode endBuff(char *buff) {
    char toCmp[10] = "END:VCARD\0";
    for(int i = 0; i < 10; i++) {
        if(*(buff + strlen(buff) - 16 + i) != toCmp[i]) {
            return INV_CARD;
        }
    }
    return OK;
}

char* upperCaseStr(char *a) {
    int j;
    if(a == NULL) {
        return NULL;
    }
    j = strlen(a);
    for(int i = 0; i < j; i++) {
        a[i] = toupper(a[i]);
    }
    return a;
}

int numSemiColons(char *a) {
    int count;

    count = 0;

    if(a == NULL) {
        return -1;
    }

    for(int i = 0; a[i] != '\0'; i++) {
        if(a[i] == ';') {
            count++;
        }
    }

    return count;
}

char *getGroup(char *token) {
    char *toReturn;
    char *group;
    char *val;

    val = strchr(token, ':');
    group = strchr(token, '.');

    if(val == NULL || group > val || group == NULL) {
        return NULL;
    }

    toReturn = malloc(sizeof(char) * (val - token ));
    strncpy(toReturn, token, val - token);
    toReturn[group - token] = '\0';

    return toReturn;
}

char *getProp(char *token) {
    char *toReturn;
    char *group;
    char *val;
    char *sc;
    val = strchr(token, ':');
    group = strchr(token, '.');

    if(group != NULL && group < val) {
        toReturn = malloc(sizeof(char) * (val - group + 1));
        strncpy(toReturn, group, val - group);
        toReturn[val - token] = '\0';
        if(numSemiColons(toReturn) != 0) {
            sc = strchr(toReturn, ';');
            *sc = '\0';
        }
        return toReturn;
    }
    else {
        toReturn = malloc(sizeof(char) * (val - token + 1));
        strncpy(toReturn, token, val - token);
        toReturn[val - token] = '\0';
        if(numSemiColons(toReturn) != 0) {
            sc = strchr(toReturn, ';');
            *sc = '\0';
        }
        return toReturn;
    }
}

char *getParam(char *token) {
    char *toReturn;
    char *group;
    char *val;
    char *sc;

    val = strchr(token, ':');
    group = strchr(token, '.');

    if(group != NULL && group < val) {
        toReturn = malloc(sizeof(char) * (val - group + 1));
        strncpy(toReturn, group, val - group);
        toReturn[val - token] = '\0';
        if(numSemiColons(toReturn) == 0) {
            free(toReturn);
            return NULL;
        }
        else {
            sc = strchr(toReturn, ';');
            sc++;
            memmove(toReturn, toReturn + (sc - toReturn), strlen(toReturn) - (sc - toReturn));
            toReturn[strlen(toReturn) - (sc - toReturn)] = '\0';
        }
        return toReturn;
    }
    else {
        toReturn = malloc(sizeof(char) * (val - token + 1));
        strncpy(toReturn, token, val - token);
        toReturn[val - token] = '\0';
        if(numSemiColons(toReturn) == 0) {
            free(toReturn);
            return NULL;
        }
        else {
            sc = strchr(toReturn, ';');
            sc++;
            memmove(toReturn, toReturn + (sc - toReturn), strlen(toReturn) - (sc - toReturn));
            toReturn[strlen(toReturn) - (sc - toReturn)] = '\0';
        }
        return toReturn;
    }
}

char *getValue(char *token) {
    char *toReturn;
    char *val;
    char *end;

    val = strchr(token, ':');
    end = token + strlen(token);

    toReturn = malloc(sizeof(char) *( end - val));
    strncpy(toReturn, val + 1, end - val);
    toReturn[end - val -1] = '\0';

    return toReturn;

}

VCardErrorCode createCard(char* fileName, Card** newCardObject) {
    //variable declarations
    FILE *fp;
    char *buffer;
    char *buffCpy;
    VCardErrorCode retVal;

    fp = fopen(fileName, "r");
    *newCardObject = malloc(sizeof(Card));

    if(fileCheck(fileName, fp) != OK) {
        fclose(fp);
        free(*newCardObject);
        newCardObject = NULL;
        return INV_FILE;
    }

    //unfold file
    buffer = unfold(fp);
    buffCpy = malloc(sizeof(char) * (strlen(buffer) + 1));
    strcpy(buffCpy, buffer);

    //parsing
    retVal = parseFile(buffCpy, newCardObject);
    if(retVal != OK) {
        fclose(fp);
        free(buffer);
        free(buffCpy);
        free(*newCardObject);
        newCardObject = NULL;
        return retVal;
    }

    //free memory
    free(buffer);
    free(buffCpy);
    fclose(fp);
    free(*newCardObject);
    return OK;
}

void deleteCard(Card* obj) {
    return;
}

char* printCard(const Card* obj) {
    return NULL;
}

char* printError(VCardErrorCode err) {
    return NULL;
}

void deleteProperty(void* toBeDeleted) {
    Property *a;

    a = (Property *)toBeDeleted;

    free(a->group);
    free(a->name);

    freeList(a->parameters);
    freeList(a->values);

    return;
}

int compareProperties(const void* first,const void* second) {
    Property *a;
    Property *b;

    a = (Property *)first;
    b = (Property *)second;
    //compare lists in property struct?
    if(strcmp(a->group, b->group) != 0 || strcmp(a->name, b->name) != 0) {
        return -1;
    }

    return 0;
}

char* printProperty(void* toBePrinted) {
    return NULL;
}

void deleteParameter(void* toBeDeleted) {
    return; 
}

int compareParameters(const void* first,const void* second) {
    return 0;
}

char* printParameter(void* toBePrinted) {
    return NULL;
}

void deleteValue(void* toBeDeleted) {
    return;
}

int compareValues(const void* first,const void* second) {
    return 0;
}

char* printValue(void* toBePrinted) {
    return NULL;
}

void deleteDate(void* toBeDeleted) {
    return;
}

int compareDates(const void* first,const void* second) {
    return 0;
}

char* printDate(void* toBePrinted) {
    return NULL;
}