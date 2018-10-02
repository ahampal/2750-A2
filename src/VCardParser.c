#include "parserHelper.h"
#include <ctype.h>

#define DEBUG 0

char* unfold(FILE *fp) {
    char *buff;
    int fileLen;

    fseek(fp, 0, SEEK_END);
    fileLen = ftell(fp);
    rewind(fp);
    buff = malloc(sizeof(char)* (fileLen + 1));

    /*
    load into memory
    remove CRLF and shift array over.
    */

    for(int i = 0; i < fileLen; i++) {
        buff[i] = fgetc(fp);
        if(i > 1) {
            if((buff[i] == ' ' || buff[i] == (char)9 ) && buff[i - 1] == '\n' && buff[i - 2] == '\r') {
                i = i - 3;
                fileLen -= 3;
            }
        }
    }

    buff[fileLen] = '\0';
    
    for(int i = 0; i < fileLen - 1; i++) {
        if(buff[i] == '\r') {
            if(buff[i+1] == '\n') {
                continue;
            }
            else {
                free(buff);
                buff = NULL;
                break;
            }
        }
    }

    return buff;
}

char *myStrChr(char *a, int c) {
    int len;

    if(a == NULL) {
        return NULL;
    }

    len = strlen(a) + 1;

    for(int i = 0; i < len; i++) {
        if(*(a+i) == c) {
            return a+i;
        }
    }

    return NULL;
    
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
    //variable declarations
    char *token = NULL;
    char *lGroup = NULL;
    char *lProp = NULL;
    char *lParam = NULL;
    char *lVal = NULL;
    Property *newProp = NULL;
    VCardErrorCode retVal;
    int groupLen;
    DateTime *newDate = NULL;
    char *tPos = NULL;
    int numContentLines = 0;
    char *buffCpy;

    //check for begin syntax

    if(beginBuff(buffer) != OK) {
        if(DEBUG) {printf("Card could not be created. Incorrect/Missing begin.\n");}
        return INV_CARD;
    }
    
    token = buffer;

    if(token == NULL) {
        return INV_CARD;
    }
    buffCpy = malloc(sizeof(char) * (strlen(buffer) + 1));
    strncpy(buffCpy, buffer, strlen(buffer) + 1);
    token = strtok(buffCpy, "\r\n");
    token = strtok(NULL, "\r\n");
    token = strtok(NULL, "\r\n");

    //parse every content line until end of vcard
    while(token != NULL) {
        if(strlen(token) > 998) {
            free(buffCpy);
            return INV_PROP;
        }
        //get group
        groupLen = 0;
        lGroup = getGroup(token);
        if(lGroup != NULL) {
            if(DEBUG) {printf("\nGroup: %s\n", lGroup);}
            retVal = checkGroup(lGroup);
            if(retVal != OK) {
                freeLine(&lGroup, &lProp, &lParam, &lVal);
                free(buffCpy);
                return retVal;
            }
            groupLen = strlen(lGroup);
        }

        //get property
        lProp = getProp(token);
        if(DEBUG) {printf("\nProperty: %s\n", lProp);}
        retVal = checkProp(lProp);
        if(retVal != OK) {
            freeLine(&lGroup, &lProp, &lParam, &lVal);
            free(buffCpy);
            return retVal;
        }

        //get parameter
        lParam = getParam(token);
        if(lParam != NULL) {
            if(DEBUG) {printf("Param: %s\n", lParam);}
            retVal = checkParam(lParam);
            if(retVal != OK) {
                freeLine(&lGroup, &lProp, &lParam, &lVal);
                free(buffCpy);
                return retVal;
            }
        }

        //get value
        lVal = getValue(token);
        retVal = checkValue(lVal);
        if(DEBUG) {printf("Value: %s\n", lVal);}
        if(retVal != OK) {
            freeLine(&lGroup, &lProp, &lParam, &lVal);
            free(buffCpy);
            return retVal;
        }

        //create property struct
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

        insertAllParams(newProp->parameters, lParam);
        insertAllValues(newProp->values, lVal);

        //place property in appropriate location
        if(strcmp(upperCaseStr(lProp), "FN") == 0) {
            if((*newCardObject)->fn == NULL) {
                (*newCardObject)->fn = newProp;
            }
            else {
                insertBack((*newCardObject)->optionalProperties, newProp);
            }
        }
        else if(strcmp(upperCaseStr(lProp), "BDAY") == 0 || strcmp(upperCaseStr(lProp), "ANNIVERSARY") == 0) {
            deleteProperty(newProp);
            newDate = malloc( sizeof(DateTime) + ( (strlen(lVal) + 1) *sizeof(char) ) );
            if(strstr(lParam, "VALUE=text") == NULL) {
                newDate->isText = false;
                strcpy(newDate->text, "\0");
                if(lVal[strlen(lVal) - 1] == 'Z') {
                    newDate->UTC = true;
                }
                else {
                    newDate->UTC = false;
                }

                tPos = strchr(lVal, 'T');
                if(tPos == NULL) {
                    strcpy(newDate->date, lVal);
                    strcpy(newDate->time, "\0");
                }

                else if(tPos == lVal) {
                    strcpy(newDate->time, lVal);
                    strcpy(newDate->date, "\0");
                }
                else {
                    strncpy(newDate->date, lVal, tPos-lVal+1);
                    newDate->date[tPos-lVal] = '\0';
                    strcpy(newDate->time, tPos);
                }
            }
            else {
                newDate->isText = true;
                newDate->UTC = false;
                strcpy(newDate->text, lVal);
                strcat(newDate->text, "\0");
                strcpy(newDate->date, "\0");
                strcpy(newDate->time, "\0");
            }
            
            if(strcmp(upperCaseStr(lProp), "BDAY") == 0) {
                (*newCardObject)->birthday = newDate;
            }
            else {
                (*newCardObject)->anniversary = newDate;
            }
            
        }
        else {
            insertBack((*newCardObject)->optionalProperties, newProp);
        }

        //free current and get next
        numContentLines++;
        freeLine(&lGroup, &lProp, &lParam, &lVal);
        token = strtok(NULL, "\r\n");
        if(endCheck(token) == OK) {
            token = NULL;
        }
        
    }

    if(endBuff(buffer) != OK) {
        if(DEBUG) {printf("Card could not be created. Incorrect/missing end.\n");}
        free(buffCpy);
        return INV_CARD;
    }

    if(numContentLines == 0) {
        if(DEBUG) {printf("Card could not be created. No Content Lines.\n");}
        free(buffCpy);
        return INV_CARD;
    }

    if((*newCardObject)->fn == NULL) {
        if(DEBUG) {printf("Card could not be created. No FN property.\n");}
        free(buffCpy);
        return INV_CARD;
    }
    free(buffCpy);
    return OK;
}

VCardErrorCode endCheck(char *a) {
    char toCmp[12] = "END:VCARD\r\n";

    if(a == NULL) {
        return OK;
    }

    for(int i = 0; a[i] != '\0'; i++) {
        if(a[i] != tolower(toCmp[i])) {
            return INV_PROP;
        }
    }

    return OK;
}

void insertAllParams(List *parList, char *lParam) {
    char *endPos = NULL;
    char *currPos = NULL;
    char *currParam = NULL;

    if(numSemiColons(lParam) == 0) {
        insertParam(parList, lParam);
    }
    else {
        endPos = strchr(lParam, ';');
        currPos = lParam;
        while(endPos != NULL) {
            currParam = malloc(sizeof(char) * (endPos - currPos + 1));
            strncpy(currParam, currPos, endPos - currPos);
            currParam[endPos - currPos] = '\0';
            insertParam(parList, currParam);
            free(currParam);
            currPos = endPos + 1;
            endPos = NULL;
            endPos = strchr(currPos, ';');
        }
        endPos = lParam + strlen(lParam);
        currParam = malloc(sizeof(char) * (endPos - currPos + 1));
        strncpy(currParam, currPos, endPos - currPos);
        currParam[endPos - currPos] = '\0';
        insertParam(parList, currParam);
        free(currParam);
    }

    return;
}

void insertAllValues(List *valList, char *lVal) {
    char *endPos;
    char *currVal;
    char *currPos;
    
    if(numSemiColons(lVal) == 0) {
        insertValue(valList, lVal);
    }
    else {
        endPos = strchr(lVal, ';');
        currPos = lVal;
        while(endPos != NULL) {
            currVal = malloc(sizeof(char) * (endPos - currPos + 1));
            strncpy(currVal, currPos, endPos - currPos);
            currVal[endPos - currPos] = '\0';
            insertValue(valList, currVal);
            free(currVal);
            currPos = endPos + 1;
            endPos = NULL;
            endPos = strchr(currPos, ';');
        }
        endPos = lVal + strlen(lVal);
        currVal = malloc(sizeof(char) * (endPos - currPos + 1));
        strncpy(currVal, currPos, endPos - currPos);
        currVal[endPos - currPos] = '\0';
        insertValue(valList, currVal);
        free(currVal);
    }

    return;
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

    if(parList == NULL || par == NULL || strcmp(par, "") == 0) {
        return;
    }

    equalSign = strchr(par, '=');
    end = strchr(par, '\0');

    if(equalSign == NULL) {
        toInsert = malloc(sizeof(Parameter) + sizeof(char));
        strncpy(toInsert->name, par, strlen(par));
        toInsert->name[strlen(par)] = '\0';
        strcpy(toInsert->value, "\0");
        insertBack(parList, toInsert);
        return;
    }

    toInsert = malloc(sizeof(Parameter) + sizeof(char) * (end - equalSign + 1));
    strncpy(toInsert->name, par, equalSign - par);
    toInsert->name[equalSign - par] = '\0';
    strncpy(toInsert->value, equalSign + 1, end - equalSign);
    strcat(toInsert->value, "\0");
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
    strcat(toInsert, "\0");
    insertBack(valList, toInsert);
    return;
}

void freeLine(char **lGroup, char **lProp, char **lParam, char **lVal) {

    if(*lGroup != NULL) {
        free(*lGroup);
        *lGroup = NULL;
    }

    if(*lProp != NULL) {
        free(*lProp);
        *lProp = NULL;
    }

    if(*lParam != NULL) {
        free(*lParam);
        *lParam = NULL;
    }

    if(*lVal != NULL) {
        free(*lVal);
        *lVal = NULL;
    }

    return;
}

VCardErrorCode checkProp(char *lProp) {

    if(lProp == NULL) {
        if(DEBUG) {printf("1\n");}
        return INV_PROP;
    }

    /*
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
    */
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
        if(DEBUG) {printf("2\n");}
        return INV_PROP;
    }

    for(int i = 0; i < strlen(lGroup); i++) {
        if(isalnum(lGroup[i]) == 0 && lGroup[i] != '-') {
            if(DEBUG) {printf("3\n");}
            return INV_PROP;
        }
    }

    return OK;
}

VCardErrorCode beginBuff(char *buff) {
    char toCmp[26] = "BEGIN:VCARD\r\nVERSION:4.0\r\n";

    if (!buff) return INV_CARD;

    for(int i = 0; toCmp[i] != '\n'; i++) {
        if(toCmp[i] != toupper(buff[i])) {
            return INV_CARD;
        }
    }
    return OK;
}

VCardErrorCode endBuff(char *buff) {
    char toCmp[14] = "\r\nEND:VCARD\r\n";
    char *ptr = NULL;
    char *buffCpy = NULL;

    if (!buff) return INV_CARD;
    buffCpy = malloc(sizeof(char) * (strlen(buff) + 1));
    strncpy(buffCpy, buff, strlen(buff) + 1);
    ptr = strstr(upperCaseStr(buffCpy), toCmp);
    if(ptr != NULL) {
        if(strcmp(upperCaseStr(ptr), toCmp) != 0) {
            free(buffCpy);
            return INV_CARD;
        }
    }
    else {
        if(DEBUG) {printf("null buff\n");}
        free(buffCpy);
        return INV_CARD;
    }
    free(buffCpy);
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
    char *sc;

    val = strchr(token, ':');
    group = strchr(token, '.');
    sc = strchr(token, ';');

    if(val == NULL || group == NULL || group > val || group > sc) {
        toReturn = malloc(sizeof(char));
        strcpy(toReturn, "\0");
        return toReturn;
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

    if(val == NULL) {
        return NULL;
    }

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
            strcpy(toReturn, "");
            return toReturn;
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
            strcpy(toReturn, "");
            return toReturn;
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
        deleteCard(*newCardObject);
        *newCardObject = NULL;
        newCardObject = NULL;
        return INV_FILE;
    }

    //unfold file
    buffer = unfold(fp);
    if(buffer == NULL) {
        fclose(fp);
        deleteCard(*newCardObject);
        *newCardObject = NULL;
        newCardObject = NULL;
        if(DEBUG) {printf("4\n");}
        return INV_PROP;    
    }
    buffCpy = malloc(sizeof(char) * (strlen(buffer) + 1));
    strcpy(buffCpy, buffer);

    //parsing
    retVal = parseFile(buffCpy, newCardObject);
    if(retVal != OK) {
        fclose(fp);
        free(buffer);
        free(buffCpy);
        deleteCard(*newCardObject);
        *newCardObject = NULL;
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
    obj = NULL;
    return;
}

char* printCard(const Card* obj) {
    char *toReturn;
    char *str;
    Property *tmp;
    ListIterator propIter;
    void *node;

    if(obj == NULL) {
        return NULL;
    }

    str = printProperty(obj->fn);
    toReturn = malloc( (sizeof(char) * (strlen(str) + 1)));
    strcpy(toReturn, str);
    strcat(toReturn, "\0");
    free(str);

    propIter = createIterator(obj->optionalProperties);
    while((node = nextElement(&propIter)) != NULL) {
        tmp = (Property *)node;
        str = printProperty(tmp);
        toReturn = realloc(toReturn, (sizeof(char) * (strlen(toReturn) + strlen(str) + 1)));
        strcat(toReturn, str);
        strcat(toReturn, "\0");
        free(str);
    }

    toReturn = realloc(toReturn, (sizeof(char) *(strlen(toReturn) + 2)));
    strcat(toReturn, "\n");

    str = printDate(obj->birthday);
    toReturn = realloc(toReturn, (sizeof(char) * (strlen(toReturn) + strlen(str) + 1)));
    strcat(toReturn, str);
    strcat(toReturn, "\0");
    free(str);

    str = printDate(obj->anniversary);
    toReturn = realloc(toReturn, (sizeof(char) * (strlen(toReturn) + strlen(str) + 1)));
    strcat(toReturn, str);
    strcat(toReturn, "\0");
    free(str);

    return toReturn;    
}

char* printError(VCardErrorCode err) {
    char *toReturn;

    switch(err) {
        case 0:
            toReturn = malloc(sizeof(char) * (strlen("OK\0") + 1) );
            strcpy(toReturn, "OK\0");
            break;
        case 1:
            toReturn = malloc(sizeof(char) * (strlen("Invalid file\0") + 1) );
            strcpy(toReturn, "Invalid file\0");
            break;
        case 2:
            toReturn = malloc(sizeof(char) * (strlen("Invalid card\0")+1));
            strcpy(toReturn, "Invalid card\0");
            break;
        case 3:
            toReturn = malloc(sizeof(char) * (strlen("Invalid property\0")+1));
            strcpy(toReturn, "Invalid property\0");
            break;     
        case 4:
            toReturn = malloc(sizeof(char) * (strlen("Write Error\0")+1));
            strcpy(toReturn, "Write Error\0");
            break;
        case 5:
            toReturn = malloc(sizeof(char) * (strlen("Invalid Error Code\0")+1));
            strcpy(toReturn, "Invalid Error Code\0");
            break;
        default:
            toReturn = malloc(sizeof(char) * strlen("what\0"));
            strcpy(toReturn, "what\0");  
    }
    return toReturn;
}

void deleteProperty(void* toBeDeleted) {

    if(toBeDeleted == NULL) {
        return;
    }

    Property *a;
    a = (Property *)toBeDeleted;
    
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
    Property *a;
    char *toReturn;
    ListIterator valueIter;
    ListIterator paramIter;
    void *node;
    char *str;
    if(toBePrinted == NULL) {
        return NULL;
    }

    a = (Property *)toBePrinted;

    toReturn = malloc(sizeof(char) * (strlen(a->group) + strlen(a->name) + 3));
    strcpy(toReturn, a->group);
    strcat(toReturn, "\n");
    strcat(toReturn, a->name);
    strcat(toReturn, "\n");
    strcat(toReturn, "\0");

    paramIter = createIterator(a->parameters);
    valueIter = createIterator(a->values);

    while((node = nextElement(&paramIter)) != NULL) {
        Parameter *tmp;
        tmp = (Parameter *)node;
        
        str = a->parameters->printData(tmp);
        toReturn = realloc(toReturn, strlen(toReturn) + strlen(str) + 1);
        strcat(toReturn, str);
        strcat(toReturn, "\0");
        free(str);
    }

    while((node = nextElement(&valueIter)) != NULL) {
        char *tmp;
        tmp = (char *)node;

        str = a->values->printData(tmp);
        toReturn = realloc(toReturn, strlen(toReturn) + strlen(str) + 2);
        strcat(toReturn, str);
        strcat(toReturn, "\n");
        strcat(toReturn, "\0");
    }

    return toReturn;
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
    char *toReturn;

    if(toBePrinted == NULL) {
        toReturn = malloc(sizeof(char));
        strcpy(toReturn, "");
        return toReturn;
    }
    
    a = (Parameter *)toBePrinted;

    toReturn = malloc(sizeof(char) * (204 + strlen(a->value)));

    strcpy(toReturn, a->name);
    strcat(toReturn, "\n");
    strcat(toReturn, a->value);
    strcat(toReturn, "\n");
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
        a = malloc(sizeof(char));
        strcpy(a, "");
        return a;
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
    DateTime *a;
    DateTime *b;

    if(first == NULL || second == NULL) {
        return -1;
    }

    a = (DateTime *)first;
    b = (DateTime *)second;

    if(a->isText == true && b->isText) {
        return strcmp(a->text, b->text);
    }
    else if(a->isText == false && b->isText == false) {
        if(strcmp(a->date, b->date) == 0 && strcmp(a->time, b->time) == 0) {
            return 0;
        }
    }
    return -1;
}

char* printDate(void* toBePrinted) {
    DateTime *a;
    char *toReturn;
    if(toBePrinted == NULL) {
        toReturn = malloc(sizeof(char));
        strcpy(toReturn, "");
        return toReturn;
    }
    
    a = (DateTime *)toBePrinted;

    if(a->isText == true) {
        toReturn = malloc(sizeof(char) * strlen(a->text) + 1);
        strcpy(toReturn, a->text);
        toReturn[strlen(a->text)] = '\0';
    }
    else {
        toReturn = malloc(sizeof(char) *(strlen(a->date) + strlen(a->time) + 3));
        strcpy(toReturn, a->date);
        strcat(toReturn, "\n");
        strcat(toReturn, a->time);
        strcat(toReturn, "\n");
        strcat(toReturn, "\0");
    }
    return toReturn;
}