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

    if(DEBUG) {printf("name Length: %d\n", fileLength);}
    if(DEBUG) {printf("extension: %s\n", extension);}

    if(extension == NULL) {
        return INV_FILE;
    }

    if(strcmp(extension, ".vcf") != 0 && strcmp(extension, ".vcard")!= 0 )  {
        if(DEBUG) {printf("Card could not be created. Improper file type.\n");}
        return INV_FILE;
    }

    return OK; 
}

VCardErrorCode parseFile(char *buffer, Card **newCardObject) {
    char *token = NULL;
    char *lGroup = NULL;
    char *lProp = NULL;
    char *lParam = NULL;
    char *lVal = NULL;
    Property *newProp = NULL;
    VCardErrorCode retVal;
    int groupLen;

    if(endBuff(buffer) != OK) {
        if(DEBUG) {printf("Card could not be created. Missing End.\n");}
        return INV_CARD;
    }
    
    token = strtok(buffer, "\r\n");
    if(token == NULL) {
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


    while(token != NULL && strcmp(token, "END:VCARD") != 0) {
        
        groupLen = 0;
        lGroup = getGroup(token);
        if(lGroup != NULL) {
            if(DEBUG) {printf("\nGroup: %s\n", lGroup);}
            retVal = checkGroup(lGroup);
            if(retVal != OK) {
                freeLine(lGroup, lProp, lParam, lVal);
                return retVal;
            }
            groupLen = strlen(lGroup);
        }

        lProp = getProp(token);
        if(DEBUG) {printf("\nProperty: %s\n", lProp);}
        retVal = checkProp(lProp);
        if(retVal != OK) {
            freeLine(lGroup, lProp, lParam, lVal);
            return retVal;
        }

        lParam = getParam(token);
        if(lParam != NULL) {
            if(DEBUG) {printf("Param: %s\n", lParam);}
            retVal = checkParam(lParam);
            if(retVal != OK) {
                freeLine(lGroup, lProp, lParam, lVal);
                return retVal;
            }
        }

        lVal = getValue(token);
        retVal = checkValue(lVal);
        if(DEBUG) {printf("Value: %s\n", lVal);}
        if(retVal != OK) {
            freeLine(lGroup, lProp, lParam, lVal);
            return retVal;
        }

        newProp = malloc(sizeof(Property));
        if(lGroup != NULL) {
            newProp->group = malloc(sizeof(char) * (strlen(lGroup) + 1));
            strcpy(newProp->group, lGroup);
        }
        else {
            newProp->group = NULL;
        }
        newProp->name = malloc(sizeof(char) * (strlen(lProp) + 1));
        newProp->parameters = initializeList(&printParameter, &deleteParameter, &compareParameters);
        newProp->values = initializeList(&printValue, &deleteValue, &compareValues);
        strcpy(newProp->name, lProp);
        strncpy(newProp->group, lGroup, groupLen);
        insertParam(newProp->parameters, lParam);
        insertValue(newProp->values, lVal);
        if(strcmp(lProp, "FN") == 0) {
            (*newCardObject)->fn = newProp;
        }
        else {
            insertBack((*newCardObject)->optionalProperties, newProp);
        }
        freeLine(lGroup, lProp, lParam, lVal);
        token = strtok(NULL, "\r\n");
        
    }
    return OK;
}

int numEqualSigns(char *a) {

    int retVal;
    
    if(a == NULL) {
        return 0;
    }

    retVal = 0;
    for(int i = 0; a[i] != '\0'; i++) {
        if(a[i] == '=') {
            retVal++;
        }
    }

    return retVal;

}
void insertParam(List *parList, char *par) {
    char *equalSign = NULL;
    char *end = NULL;
    Parameter *toInsert = NULL;

    if(parList == NULL || par == NULL) {
        return;
    }

    equalSign = strchr(par, '=');
    end = par + strlen(par);

    toInsert = malloc(sizeof(Parameter) + sizeof(char) * (end - equalSign + 1));
    strncpy(toInsert->name, par, equalSign - par);
    strncpy(toInsert->value, par + strlen(equalSign), end - equalSign);
    insertBack(parList, toInsert);
    return;
}

void insertValue(List *valList, char *lVal) {
    char *toInsert;

    if(valList == NULL || lVal == NULL) {
        return;
    }

    toInsert = malloc(sizeof(char) * (strlen(lVal) + 1));
    strcpy(toInsert, lVal);
    insertBack(valList, toInsert);
    return;
}

void freeLine(char *lGroup, char *lProp, char *lParam, char *lVal) {

    if(lGroup != NULL) {
        free(lGroup);
        lGroup = NULL;
    }

    if(lProp != NULL) {
        free(lProp);
        lProp = NULL;
    }

    if(lParam != NULL) {
        free(lParam);
        lParam = NULL;
    }

    if(lVal != NULL) {
        free(lVal);
        lVal = NULL;
    }

    return;
}

VCardErrorCode checkProp(char *lProp) {

    if(lProp == NULL) {
        return INV_PROP;
    }

    int flag;

    char name[35][15]  = {"SOURCE\0", "KIND\0" , "FN\0" , "N\0" , "NICKNAME\0"
        , "PHOTO\0" , "BDAY\0" , "ANNIVERSARY\0" , "GENDER\0" , "ADR\0" , "TEL\0"
        , "EMAIL\0" , "IMPP\0" , "LANG\0" , "TZ\0" , "GEO\0" , "TITLE\0" , "ROLE\0"
        , "LOGO" , "ORG" , "MEMBER" , "RELATED" , "CATEGORIES"
        , "NOTE\0" , "PRODID\0" , "REV\0" , "SOUND\0" , "UID\0" , "CLIENTPIDMAP\0"
        , "URL\0" , "KEY\0" , "FBURL\0" , "CALADRURI\0" , "CALURI\0" , "XML\0"};

    flag = 0;

    for(int i = 0; i < 35; i++) {
        if(strcmp(upperCaseStr(name[i]), lProp) == 0) {
            flag = 1;
            break;
        }
    }
    if(flag != 1) {
        return INV_PROP;
    }
    return OK;
}

VCardErrorCode checkParam(char *lParam) {
    return OK;
}

VCardErrorCode checkValue(char *lVal) {
    return OK;
}

VCardErrorCode checkGroup(char *lGroup) {

    if (strlen(lGroup) < 0) {
        return INV_PROP;
    }

    for(int i = 0; i < strlen(lGroup); i++) {
        if(isalnum(lGroup[i]) == 0 && lGroup[i] != '-') {
            return INV_PROP;
        }
    }

    return OK;
}

VCardErrorCode endBuff(char *buff) {
    char toCmp[10] = "END:VCARD\0";
    for(int i = 0; i < 9; i++) {
        if(buff[strlen(buff) -16 + i] != toCmp[i]) {
            if(DEBUG) {printf("%s\n", buff + strlen(buff) - 16);}
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
        return 0;
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
    char *toReturn = NULL;
    char *group = NULL;
    char *val = NULL;
    char *sc = NULL;

    if(token == NULL) {
        return NULL;
    }

    val = strchr(token, ':');
    group = strchr(token, '.');

    if(group != NULL && group < val) {
        toReturn = malloc(sizeof(char) * (val - group + 1));
        strncpy(toReturn, group + 1, val - group - 1);
        toReturn[val - group - 1] = '\0';
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
    char *toReturn = NULL;
    char *group = NULL;
    char *val = NULL;
    char *sc = NULL;

    val = strchr(token, ':');
    group = strchr(token, '.');

    if(group != NULL && group < val) {
        toReturn = malloc(sizeof(char) * (val - group + 1));
        strncpy(toReturn, group, val - group);
        toReturn[val - group] = '\0';
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
    (*newCardObject)->optionalProperties = initializeList(&printProperty, &deleteProperty, &compareProperties);
    (*newCardObject)->birthday = NULL;
    (*newCardObject)->anniversary = NULL;
    (*newCardObject)->fn = NULL;
    if(fileCheck(fileName, fp) != OK) {
        fclose(fp);
        deleteCard(*newCardObject);
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
        deleteCard(*newCardObject);
        newCardObject = NULL;
        return retVal;
    }
    free(buffer);
    free(buffCpy);
    fclose(fp);
    return OK;
}

void deleteCard(Card* obj) {

    if(obj == NULL) {
        return;
    }
    
    deleteProperty(obj->fn);
    freeList(obj->optionalProperties);
    deleteDate(obj->birthday);
    deleteDate(obj->anniversary);
    free(obj);
    
    return;
}

char* printCard(const Card* obj) {

    if(obj == NULL) {
        return NULL;
    }

    return NULL;    
}

char* printError(VCardErrorCode err) {
    return NULL;
}

void deleteProperty(void* toBeDeleted) {

    Property *a;
    a = (Property *)toBeDeleted;

    if(toBeDeleted == NULL) {
        return;
    }
    
    if(a->group != NULL) {
        free(a->group);
    }

    if(a->name != NULL) {
        free(a->name);
    }

    a->group = NULL;
    a->name = NULL;

    freeList(a->parameters);
    freeList(a->values);

    a->parameters = NULL;
    a->values = NULL;

    free(a);

    a = NULL;

    return;
}

int compareProperties(const void* first,const void* second) {
    Property *a;
    Property *b;

    if(first == NULL || second == NULL) {
        return -1;
    }

    a = (Property *)first;
    b = (Property *)second;
    //compare lists in property struct?
    if(strcmp(a->name, b->name) != 0) {
        return 1;
    }

    return 0;
}

char* printProperty(void* toBePrinted) {
    return NULL;
}

void deleteParameter(void* toBeDeleted) {

    Parameter *a;
    a = (Parameter *)toBeDeleted;

    if(toBeDeleted == NULL) {
        return;
    }

    free(a);

    return; 
}

int compareParameters(const void* first,const void* second) {

    Parameter *a;
    Parameter *b;

    a = (Parameter *)first;
    b = (Parameter *)second;

    if(first == NULL || second == NULL) {
        return 1;
    }

    if(strcmp(a->name, b->name) != 0 || strcmp(a->value, b->value) != 0) {
        return 1;
    }
    return 0;
}

char* printParameter(void* toBePrinted) {

    Parameter *a;
    a = (Parameter *)toBePrinted;

    char *toReturn;

    toReturn = malloc(sizeof(char) * (220 + strlen(a->value)));

    strcpy(toReturn, "Name: ");
    strcat(toReturn, a->name);
    strcat(toReturn, "\nValue: ");
    strcat(toReturn, a->value);
    strcat(toReturn, "\0");

    return toReturn;
}

void deleteValue(void* toBeDeleted) {

    char *a;

    if(toBeDeleted == NULL) {
        return;
    }

    a = (char *)toBeDeleted;

    free(a);

    return;
}

int compareValues(const void* first,const void* second) {

    char *a;
    char *b;

    if(first == NULL || second == NULL) {
        return 0;
    }

    a = (char *)first;
    b = (char *)second;

    if(strcmp(a,b) != 0) {
        return 0;
    }

    return 1;
}

char* printValue(void* toBePrinted) {
    
    char *a;

    if(toBePrinted == NULL) {
        return NULL;
    }

    a = (char *)toBePrinted;

    return a;
}

void deleteDate(void* toBeDeleted) {

    if(toBeDeleted == NULL) {
        return;
    }

    DateTime *a;
    a = (DateTime *)toBeDeleted;

    free(a);

    return;
}

int compareDates(const void* first,const void* second) {
    return 0;
}

char* printDate(void* toBePrinted) {
    return NULL;
}