#include "parserHelper.h"
#include <ctype.h>

#define DEBUG 0 
#define CREATECARD_ERROR_CHECKING 0
//Turning error checking off may result in crashes but is necessary for
//creating invalid cards to be used for testing validateCard

//A2 functions
Card* JSONtoCard(const char* str) {
    if(str == NULL || *str != '{' || str[strlen(str) - 1] != '}') {
        return NULL;
    }

    Card *toReturn = NULL;
    char *tmpOne = NULL;
    char *tmpTwo = NULL;
    char *castedStr = NULL;
    char *value = NULL;
    Property *fnProp = NULL;

    toReturn = malloc(sizeof(toReturn));
    castedStr = (char *)str;

    toReturn->birthday = NULL;
    toReturn->anniversary = NULL;
    toReturn->optionalProperties = initializeList(&printProperty, &deleteProperty, &compareProperties);

    tmpOne = myStrChr(castedStr, '\"');
    tmpTwo = myStrChr(tmpOne + 1, '\"');
    tmpOne = myStrChr(tmpTwo + 1, '\"');
    tmpTwo = myStrChr(tmpOne + 1, '\"');

    value = malloc(sizeof(char) * (tmpTwo - tmpOne + 1));
    strncpy(value, tmpOne + 1, tmpTwo - tmpOne - 1);
    value[tmpTwo - tmpOne - 1] = '\0';

    fnProp = malloc(sizeof(Property));
    fnProp->group = malloc(sizeof(char));
    strcpy(fnProp->group, "");

    fnProp->name = malloc(sizeof(char) * (strlen("FN\0")));
    strcpy(fnProp->name, "FN\0");

    fnProp->parameters = initializeList(&printParameter, &deleteParameter, &compareParameters);
    fnProp->values = initializeList(&printValue, &deleteValue, &compareValues);
    insertBack(fnProp->values, value);

    toReturn->fn = fnProp;

    return toReturn;

}

DateTime* JSONtoDT(const char* str) {
    if(str == NULL) {
        return NULL;
    }

    DateTime *toReturn;
    char *castedStr;
    char *tmpOne;
    char *tmpTwo;
    char *val;

    toReturn = malloc(sizeof(DateTime));
    castedStr = (char *)str;
    tmpOne = myStrChr(castedStr, ':');
    tmpTwo = myStrChr(tmpOne, ',');

    if(*castedStr != '{' || castedStr[strlen(castedStr) - 1] != '}') {
        return NULL;
    }

    val = malloc(sizeof(char) * (tmpTwo - tmpOne + 1));
    strncpy(val, tmpOne + 1, tmpTwo - tmpOne - 1);
    val[tmpTwo - tmpOne - 1] = '\0';

    if(strcmp(val, "false") == 0) {
        free(val);
        toReturn = realloc(toReturn, sizeof(DateTime) + sizeof(char));
        toReturn->isText = false;
        toReturn->UTC = true;
        strcpy(toReturn->text, "");

        tmpOne = myStrChr(tmpOne + 1, ':');
        tmpTwo = myStrChr(tmpOne + 1, '\"');

        val = malloc(sizeof(char) * (tmpTwo - tmpOne + 1));
        strncpy(val, tmpOne + 1, tmpTwo - tmpOne - 1);
        val[tmpTwo - tmpOne - 1] = '\0';

        strcpy(toReturn->date, val);
        free(val);

        tmpOne = myStrChr(tmpOne + 1, ':');
        tmpTwo = myStrChr(tmpOne + 1, '\"');

        val = malloc(sizeof(char) * (tmpTwo - tmpOne + 1));
        strncpy(val, tmpOne + 1, tmpTwo - tmpOne - 1);
        val[tmpTwo - tmpOne - 1] = '\0';

        strcpy(toReturn->time, val);
        free(val);
    }
    else {
        free(val);
        toReturn->isText = true;
        toReturn->UTC = false;

        strcpy(toReturn->time, "");
        strcpy(toReturn->date, "");

        tmpOne = myStrChr(tmpOne, ':');
        tmpOne = myStrChr(tmpOne, ':');
        tmpOne = myStrChr(tmpOne, ':');
        tmpTwo = myStrChr(tmpOne + 1, '\"');

        free(val);
        val = malloc(sizeof(char) * (tmpTwo - tmpOne + 1));
        strncpy(val, tmpOne + 1, tmpTwo - tmpOne - 1);
        val[tmpTwo - tmpOne - 1] = '\0';

        toReturn = realloc(toReturn, sizeof(DateTime) + strlen(val));
        strcpy(toReturn->text, val);

        free(val);
    }

    return toReturn;
}

void addProperty(Card* card, const Property* toBeAdded) {

    if(card == NULL || toBeAdded == NULL) {
        return;
    }
    Property *castedProp = (Property *)toBeAdded;
    /*
    Property *prop;
    ListIterator paramIter;
    ListIterator valIter;
    void *tmp;
    Parameter *currParam;
    Parameter *paramToBeAdded;
    char *currVal;
    char *valToBeAdded;

    prop = malloc(sizeof(Property));
    prop->group = malloc(sizeof(char) * (strlen(toBeAdded->group) + 1));
    prop->name = malloc(sizeof(char) * (strlen(toBeAdded->name) + 1));
    prop->parameters = initializeList(&printParameter, &deleteParameter, &compareParameters);
    prop->values = initializeList(&printValue, &deleteValue, &compareValues);

    strcpy(prop->group, toBeAdded->group);
    strcat(prop->group, "\0");

    strcpy(prop->name, toBeAdded->name);
    strcat(prop->name, "\0");

    paramIter = createIterator(toBeAdded->parameters);
    while((tmp = nextElement(&paramIter)) != NULL) {
        currParam = (Parameter *)tmp;
        paramToBeAdded = malloc(sizeof(Parameter) + (sizeof(char)*(strlen(currParam->value)+1)) );
        strcpy(paramToBeAdded->name, currParam->name);
        strcpy(paramToBeAdded->value, currParam->value);
        insertBack(prop->parameters, paramToBeAdded);
        paramToBeAdded++;
    }

    valIter = createIterator(toBeAdded->values);
    while((tmp = nextElement(&valIter)) != NULL) {
        currVal = (char *)tmp;
        valToBeAdded = malloc((sizeof(char)*(strlen(currVal)+1)) );
        strcpy(valToBeAdded, currVal);
        strcat(valToBeAdded, "\0");
        insertBack(prop->values, valToBeAdded);
        valToBeAdded++;
    }
    */
    insertBack(card->optionalProperties, castedProp);

    return;
}

char* dtToJSON(const DateTime* prop) {

    DateTime *castedProp;
    char *toReturn;
    castedProp = (DateTime *)prop;

    if(castedProp == NULL) {
        toReturn = malloc(sizeof(char));
        strcpy(toReturn, "\0");
        return toReturn;
    }

    toReturn = malloc(sizeof(char) * (strlen("{\"isText\":\0") + 1));
    strcpy(toReturn, "{\"isText\":\0");
    if(castedProp->isText == true) {
        toReturn = realloc(toReturn, strlen(toReturn) + strlen("true,\"date\":\"\0"));
        strcat(toReturn, "true,\"date\":\"\0");
    }
    else {
        toReturn = realloc(toReturn, strlen(toReturn) + strlen("false,\"date\":\"\0") + 1);
        strcat(toReturn, "false,\"date\":\"\0");    
    }

    toReturn = realloc(toReturn, strlen(toReturn) + (strlen(castedProp->date) + 1));
    strcat(toReturn, castedProp->date);
    strcat(toReturn, "\0");

    toReturn = realloc(toReturn, strlen(toReturn) + (strlen("\",\"time\":\"\0") + 1));
    strcat(toReturn, "\",\"time\":\"\0");
    
    toReturn = realloc(toReturn, strlen(toReturn) + (strlen(castedProp->time) + 1));
    strcat(toReturn, castedProp->time);
    strcat(toReturn, "\0");

    toReturn = realloc(toReturn, strlen(toReturn) + strlen("\",\"text\":\"") + 2);
    strcat(toReturn, "\",\"text\":\"");

    toReturn = realloc(toReturn, strlen(toReturn) + strlen(castedProp->text) + 1);
    strcat(toReturn, castedProp->text);
    strcat(toReturn, "\0");

    toReturn = realloc(toReturn, strlen(toReturn) + strlen("\",\"isUTC\":") + 1);
    strcat(toReturn, "\",\"isUTC\":\0");

    if(castedProp->UTC == true) {
        toReturn = realloc(toReturn, strlen(toReturn) + strlen("true}\0"));
        strcat(toReturn, "true}\0");
    }
    else {
        toReturn = realloc(toReturn, strlen(toReturn) + strlen("false}\0") + 1);
        strcat(toReturn, "false}\0");  
    }

    return toReturn;
}

Property* JSONtoProp(const char* str) {

    if(str == NULL) return NULL;
    if(*str != '{') return NULL;
    if(str[strlen(str) - 1] != '}') return NULL;

    char *dQuoteOne = NULL;
    char *dQuoteTwo = NULL;
    char *valJSON = NULL;
    char *castedStr = (char *)str;
    Property *toReturn;

    toReturn = malloc(sizeof(Property));
    dQuoteOne = castedStr + 9;
    dQuoteTwo = myStrChr(dQuoteOne + 1, '\"');
    toReturn->group = malloc(sizeof(char) *(dQuoteTwo - dQuoteOne + 1));
    strncpy(toReturn->group, dQuoteOne + 1, dQuoteTwo - dQuoteOne - 1);
    toReturn->group[dQuoteTwo - dQuoteOne - 1] = '\0';

    dQuoteOne = dQuoteTwo + 9;
    dQuoteTwo = myStrChr(dQuoteOne + 1, '\"');
    toReturn->name = malloc(sizeof(char) * (dQuoteTwo - dQuoteOne + 1));
    strncpy(toReturn->name, dQuoteOne + 1, dQuoteTwo - dQuoteOne - 1);
    toReturn->name[dQuoteTwo - dQuoteOne - 1] = '\0';

    dQuoteOne = dQuoteTwo + 10;
    dQuoteTwo = castedStr + strlen(castedStr) - 2;
    valJSON = malloc(sizeof(char) * (dQuoteTwo - dQuoteOne + 1));
    strncpy(valJSON, dQuoteOne + 1, dQuoteTwo - dQuoteOne);
    valJSON[dQuoteTwo - dQuoteOne] = '\0';
    toReturn->values = JSONtoStrList(valJSON);
    free(valJSON);

    toReturn->parameters = initializeList(&printParameter, &deleteParameter, &compareParameters);

    return toReturn;

}

char* propToJSON(const Property* prop) {

    char *toReturn;
    char *valJSON;
    Property *castedProp = (Property *)prop;

    if(castedProp == NULL || castedProp->group == NULL || castedProp->name == NULL || castedProp->parameters == NULL || castedProp->values == NULL) {
        toReturn = malloc(sizeof(char));
        strcpy(toReturn, "\0");
        return toReturn;
    }

    toReturn = malloc(sizeof(char) * 11);
    strcpy(toReturn, "{\"group\":\"\0");
    
    //add group
    toReturn = realloc(toReturn, strlen(toReturn) + strlen(prop->group) + strlen("\",\"name\":\"\0") + 1);
    strcat(toReturn, prop->group);
    strcat(toReturn, "\",\"name\":\"\0");

    //add prop
    toReturn = realloc(toReturn, strlen(toReturn) + strlen(prop->name) + strlen("\",\"values\":\0") + 1);
    strcat(toReturn, prop->name);
    strcat(toReturn, "\",\"values\":\0");

    //add values
    valJSON = strListToJSON(prop->values);
    toReturn = realloc(toReturn, strlen(toReturn) + strlen(valJSON) + 2);
    strcat(toReturn, valJSON);
    strcat(toReturn, "}\0");
    free(valJSON);

    return toReturn;
}

char* strListToJSON(const List* strList) {

    if(strList == NULL) return NULL;

    char *toReturn;
    List *castedList;
    int listLen;
    ListIterator strIter;
    void *node;
    char *currStr;
    int currStrLen;

    castedList = (List *)strList;
    listLen = getLength(castedList);

    if(listLen == 0) {
        toReturn = malloc(sizeof(char) * 3);
        strcpy(toReturn,"[]\0");
        return toReturn;
    }

    toReturn = malloc(sizeof(char) * 2);
    strcpy(toReturn, "[\0");
    strIter = createIterator(castedList);

    while((node = nextElement(&strIter)) != NULL) {
        currStr = (char *)node;
        currStrLen = strlen(currStr);
        toReturn = realloc(toReturn, strlen(toReturn) + currStrLen + 4);
        strcat(toReturn, "\"");
        strcat(toReturn, currStr);
        strcat(toReturn, "\",\0");
    }

    toReturn[strlen(toReturn) - 1] = ']';

    return toReturn;
}

List* JSONtoStrList(const char* str) {

    if(str == NULL) return NULL;
    if(*str != '[') return NULL;
    if(str[strlen(str) - 1] != ']') return NULL;

    char *dQuoteOne = NULL;
    char *dQuoteTwo = NULL;
    char *listVal = NULL;
    char *castedStr = NULL;
    List *toReturn;

    castedStr = (char *)str;
    dQuoteOne = myStrChr(castedStr, '\"');
    dQuoteTwo = myStrChr(dQuoteOne + 1, '\"');

    toReturn = initializeList(&printValue, &deleteValue, &compareValues);
    while(dQuoteOne != NULL && dQuoteTwo != NULL) {
        listVal = malloc(sizeof(char) * (dQuoteTwo - dQuoteOne + 1));
        strncpy(listVal, dQuoteOne + 1, dQuoteTwo - dQuoteOne - 1);
        listVal[dQuoteTwo - dQuoteOne - 1] = '\0';
        insertBack(toReturn, listVal);
        listVal++;
        dQuoteOne = myStrChr(dQuoteTwo + 1, '\"');
        if(dQuoteOne != NULL) {
            dQuoteTwo = myStrChr(dQuoteOne + 1, '\"');
        }
        else {
            dQuoteTwo = NULL;
        }
    }
    return toReturn;
}

VCardErrorCode writeCard(const char* fileName, const Card* obj) {

    if(fileName == NULL) return WRITE_ERROR;
    if(obj == NULL) return WRITE_ERROR;

    FILE *fp = NULL;
    char *fNameCpy = NULL;
    char *line = NULL;
    ListIterator propIter;
    void *node;
    Property *tmp;

    fp = fopen(fileName, "w");
    fNameCpy = malloc(sizeof(char) * (strlen(fileName) + 1));
    strncpy(fNameCpy, fileName, strlen(fileName));
    fNameCpy[strlen(fileName)] = '\0';
    if(fileCheck(fNameCpy, fp) != OK) {
        free(fNameCpy);
        fNameCpy = NULL;
        fclose(fp);
        return WRITE_ERROR;
    }
    free(fNameCpy);
    fNameCpy = NULL;
    
    fprintf(fp, "BEGIN:VCARD\r\nVERSION:4.0\r\n");
    line = printProperty(obj->fn);
    fprintf(fp, "%s\r\n", line);
    free(line);

    propIter = createIterator(obj->optionalProperties);
    while((node = nextElement(&propIter)) != NULL) {
        tmp = (Property *)node;
        line = printProperty(tmp);
        fprintf(fp, "%s\r\n", line);
        free(line);
    }

    if(obj->birthday != NULL) {
        fprintf(fp, "BDAY:");
        line = printDate(obj->birthday);
        fprintf(fp, "%s\r\n", line);
        free(line);
    }

    if(obj->anniversary != NULL) {
        fprintf(fp, "ANNIVERSARY:");
        line = printDate(obj->anniversary);
        fprintf(fp, "%s\r\n", line);
        free(line);
    }

    fprintf(fp, "END:VCARD\r\n");

    fclose(fp);
    return OK;
}

VCardErrorCode checkPropStruct(Property *a) {

    if(a == NULL) {
        return INV_PROP;
    }

    Property *toCheck = NULL;
    char *cmpStr = NULL;
    VCardErrorCode retVal;

    toCheck = (Property *)a;

    if(toCheck->name == NULL) return INV_PROP;
    if(strcmp(toCheck->name, "") == 0) return INV_PROP;
    if(toCheck->group == NULL) return INV_PROP;
    if(toCheck->parameters == NULL) return INV_PROP;
    if(toCheck->values == NULL) return INV_PROP;
    if(getLength(toCheck->values) < 1) return INV_PROP;

    cmpStr = malloc(sizeof(char) * (strlen(toCheck->name) + 1));
    strcpy(cmpStr, toCheck->name);
    strcat(cmpStr, "\0");
    cmpStr = upperCaseStr(cmpStr);
    retVal = checkPropName(cmpStr);
    free(cmpStr);
    if(retVal != OK) return retVal;

    retVal = checkPropVal(toCheck->values);
    if(retVal != OK) return retVal;

    retVal = checkParamList(toCheck->parameters);
    if(retVal != OK) return retVal;
    
    retVal = checkValCardinality(toCheck);
    if(retVal != OK) return retVal;

    return OK; 
}

VCardErrorCode checkValCardinality(Property *toCheck) {

    if(toCheck == NULL) return INV_PROP;

    char *currProp = NULL;

    currProp = malloc(sizeof(char) * (strlen(toCheck->name) + 1));
    strcpy(currProp, toCheck->name);
    strcat(currProp, "\0");
    currProp = upperCaseStr(currProp);

    if(strcmp(currProp, "SOURCE\0") == 0) {
        if(getLength(toCheck->values) < 0 || getLength(toCheck->values) > 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "KIND\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "XML\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "FN\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "N\0") == 0) {
        if(getLength(toCheck->values) != 5) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "NICKNAME\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "PHOTO\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "GENDER\0") == 0) {
        if(getLength(toCheck->values) < 1 || getLength(toCheck->values) > 2) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "ADR\0") == 0) {
        if(getLength(toCheck->values) != 7) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "TEL\0") == 0) {
        if(getLength(toCheck->values) < 1 || getLength(toCheck->values) > 28) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "EMAIL\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "IMPP\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "LANG\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "TZ\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "GEO\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "TEXT\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "ROLE\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "LOGO\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "ORG\0") == 0) {
        if(getLength(toCheck->values) < 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "MEMBER\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "RELATED\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "CATEGORIES\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "NOTE\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }  

    if(strcmp(currProp, "PRODID\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "REV\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "SOUND\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }           

    if(strcmp(currProp, "UID\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "CLIENTPIDMAP\0") == 0) {
        if(getLength(toCheck->values) != 2) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "URL\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "KEY\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "FBURL\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "CALADURI\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }

    if(strcmp(currProp, "CALURI\0") == 0) {
        if(getLength(toCheck->values) != 1) {
            free(currProp);
            return INV_PROP;
        }
    }
    free(currProp);
    return OK;
}

VCardErrorCode checkParamList(List *a) {
    Parameter *currParam;
    void *node;
    ListIterator paramListIter;

    paramListIter = createIterator(a);
    while( (node = nextElement(&paramListIter)) != NULL) {
        currParam = (Parameter *)node;
        if(currParam == NULL) return INV_PROP;
        if(strcmp(currParam->name, "") == 0) return INV_PROP;
        if(strcmp(currParam->value, "") == 0) return INV_PROP;
    }

    return OK;
}

VCardErrorCode checkPropVal(List *a) {
    char *currVal;
    void *node;
    ListIterator valListIter;

    valListIter = createIterator(a);
    while( (node = nextElement(&valListIter)) != NULL ) {
        currVal = (char *)node;
        if(currVal == NULL) return INV_PROP;
    }

    return OK;
}

VCardErrorCode checkPropName(char *a) {
    char name[35][15]  = {"SOURCE\0", "KIND\0" , "FN\0" , "N\0" , "NICKNAME\0"
        , "PHOTO\0" , "BDAY\0" , "ANNIVERSARY\0" , "GENDER\0" , "ADR\0" , "TEL\0"
        , "EMAIL\0" , "IMPP\0" , "LANG\0" , "TZ\0" , "GEO\0" , "TITLE\0" , "ROLE\0"
        , "LOGO\0" , "ORG\0" , "MEMBER\0" , "RELATED\0" , "CATEGORIES\0"
        , "NOTE\0" , "PRODID\0" , "REV\0" , "SOUND\0" , "UID\0" , "CLIENTPIDMAP\0"
        , "URL\0" , "KEY\0" , "FBURL\0" , "CALADRURI\0" , "CALURI\0" , "XML\0"};

    if(a == NULL) return INV_PROP;
    if(strcmp(a, "VERSION") == 0) return INV_CARD;
    if(strcmp(a, "BDAY") == 0) return INV_DT;
    if(strcmp(a, "ANNIVERSARY") == 0) return INV_DT;

    for(int i = 0; i < 35; i++) {
        if(strcmp(name[i], a) == 0) return OK;
    }
    return INV_PROP;
}

VCardErrorCode checkDTStruct(DateTime *a) {

    if(a == NULL) return OK;
    if(a->text == NULL) return INV_DT;
    if(a->time == NULL) return INV_DT;

    DateTime *toCheck = NULL;
    toCheck = (DateTime *)a;

    if(toCheck->isText == true && strcmp(toCheck->date, "") != 0) return INV_DT;
    if(toCheck->isText == true && strcmp(toCheck->time, "") != 0) return INV_DT;
    if(toCheck->isText == false && strcmp(toCheck->text, "") != 0) return INV_DT;
    if(toCheck->isText == false && toCheck->UTC == true) return INV_DT;

    if(toCheck->isText == false) {
        if(strcmp(toCheck->date, "") == 0 && strcmp(toCheck->time, "") == 0) return INV_DT;
        if(dateFormatCheck(toCheck->date) != OK) return INV_DT;
        if(timeFormatCheck(toCheck->time) != OK) return INV_DT;
    }

    return OK;

}

VCardErrorCode timeFormatCheck(char *toCheck) {

    if(toCheck == NULL) return INV_DT;
    if(strcmp(toCheck, "") == 0) return OK;
    if(strlen(toCheck) > 6) return INV_DT;

    char *hour = malloc(sizeof(char) * 3);
    char *min = malloc(sizeof(char) * 3);
    char *sec = malloc(sizeof(char) * 3);
    VCardErrorCode secRetVal;
    VCardErrorCode minRetVal;
    VCardErrorCode hourRetVal;

    strcpy(hour, "-1\0");
    strcpy(min, "-1\0");
    strcpy(sec, "-1\0");

    for(int i = 0; toCheck[i] != '\0'; i++) {
        if(isdigit(toCheck[i]) == 0 && toCheck[i] != '-') {
            free(hour);
            free(min);
            free(sec);
            return INV_DT;
        }
    }

    if(strncmp(toCheck, "--", 2) == 0) {
        strncpy(sec, toCheck + 2, 2);
        strcat(sec, "\0");
        secRetVal = secCheck(sec);
        free(hour);
        free(min);
        free(sec);
        return secRetVal;
    }
    else if(strncmp(toCheck, "-", 1) == 0) {
        strncpy(min, toCheck + 1, 2);
        strcat(min, "\0");
        if(strlen(toCheck) > 3 && strlen(toCheck) >= 5) {
            strncpy(sec, toCheck + 3, 2);
            strcat(sec, "\0");
        }
        secRetVal = secCheck(sec);
        minRetVal = minCheck(min);
        free(sec);
        free(min);
        free(hour);
        if(strlen(toCheck) > 3) {
            return secRetVal + minRetVal;
        }
        return minRetVal;
    }
    else {
        strncpy(hour, toCheck, 2);
        strcat(hour, "\0");
        if(strlen(toCheck) > 2 && strlen(toCheck) >= 4) {
            strncpy(min, toCheck + 2, 2);
            strcat(min, "\0");
            if(strlen(toCheck) > 4 && strlen(toCheck) >= 6) {
                strncpy(sec, toCheck + 4, 2);
                strcat(sec, "\0");
            }
        }
        secRetVal = secCheck(sec);
        minRetVal = minCheck(min);
        hourRetVal = hourCheck(hour);
        free(sec);
        free(min);
        free(hour);
        if(strlen(toCheck) > 2) {
            if(strlen(toCheck) > 4) {
                return hourRetVal + minRetVal + secRetVal;
            }
            return hourRetVal + minRetVal;
        }
        return hourRetVal;
    }
    return INV_DT;
}

VCardErrorCode secCheck(char *a) {
    if(a == NULL) return INV_DT;
    if(strcmp(a, "") == 0) return INV_DT;
    if(strlen(a) != 2) return INV_DT;

    double month = 0;
    month = strtol(a, NULL, 10);

    if(month < 0 || month > 60) return INV_DT;

    return OK;
}

VCardErrorCode minCheck(char *a) {
    if(a == NULL) return INV_DT;
    if(strcmp(a, "") == 0) return INV_DT;
    if(strlen(a) != 2) return INV_DT;

    double month = 0;
    month = strtol(a, NULL, 10);

    if(month < 0 || month > 59) return INV_DT;

    return OK;
}

VCardErrorCode hourCheck(char *a) {
    if(a == NULL) return INV_DT;
    if(strcmp(a, "") == 0) return INV_DT;
    if(strlen(a) != 2) return INV_DT;

    double month = 0;
    month = strtol(a, NULL, 10);

    if(month < 0 || month > 23) return INV_DT;

    return OK;
}

VCardErrorCode dateFormatCheck(char *toCheck) {

    if(toCheck == NULL) return INV_DT;
    if(strcmp(toCheck, "") == 0) return OK;
    if(strlen(toCheck) > 8) return INV_DT;

    char *tmp;
    char *year = malloc(sizeof(char) * 5);
    char *month = malloc(sizeof(char) * 3);
    char *day = malloc(sizeof(char) * 3);
    VCardErrorCode dayRetVal;
    VCardErrorCode monthRetVal;
    VCardErrorCode yearRetVal;

    strcpy(year, "-234\0");
    strcpy(month, "00\0");
    strcpy(day, "00\0");

    for(int i = 0; toCheck[i] != '\0'; i++) {
        if(isdigit(toCheck[i]) == 0 && toCheck[i] != '-') {
            free(year);
            free(month);
            free(day);
            return INV_DT;
        }
    }

    if(strncmp(toCheck, "---", 3) == 0) {
        strncpy(day, toCheck + 3, 2);
        strcat(day, "\0");
        dayRetVal = dayCheck(day);
        free(year);
        free(month);
        free(day);
        return dayRetVal;
    }
    else if(strncmp(toCheck, "--", 2) == 0) {
        strncpy(month, toCheck + 2, 2);
        strcat(month, "\0");
        if(strlen(toCheck) > 4) {
            strncpy(day, toCheck + 4, 2);
            strcat(day, "\0");
        }
        monthRetVal = monthCheck(month);
        dayRetVal = dayCheck(day);
        free(year);
        free(month);
        free(day);
        if(strlen(toCheck) > 4) {
            return monthRetVal + dayRetVal;
        }
        return monthRetVal;
    }
    else if( (tmp = myStrChr(toCheck, '-')) != NULL) {
        strncpy(year, toCheck, 4);
        strcat(year, "\0");
        strncpy(month, tmp + 1, 2);
        strcat(month, "\0");
        yearRetVal = yearCheck(year);
        monthRetVal = monthCheck(month);
        free(year);
        free(month);
        free(day);
        return yearRetVal + monthRetVal;
    } 
    else {
        strncpy(year, toCheck, 4);
        strcat(year, "\0");
        if(strlen(toCheck) > 4 && strlen(toCheck) >= 6) {
            strncpy(month, toCheck + 4, 2);
            strcat(month, "\0");
            if(strlen(toCheck) > 6 && strlen(toCheck) >= 8) {
                strncpy(day, toCheck + 6, 2);
                strcat(day, "\0");
            }
        }
        dayRetVal = dayCheck(day);
        yearRetVal = yearCheck(year);
        monthRetVal = monthCheck(month);
        free(year);
        free(month);
        free(day);
        if(strlen(toCheck) > 4) {
            return dayRetVal + monthRetVal + yearRetVal;
        }
        return yearRetVal;
    }

    free(year);
    free(month);
    free(day);
    return INV_DT;
}

VCardErrorCode yearCheck(char *a) {
    if(a == NULL) return INV_DT;
    if(strcmp(a, "") == 0) return INV_DT;
    if(strlen(a) != 4) return INV_DT;

    double month = 0;
    month = strtol(a, NULL, 10);

    if(month < 0 || month > 9999) return INV_DT;

    return OK;
}

VCardErrorCode monthCheck(char *a) {
    if(a == NULL) return INV_DT;
    if(strcmp(a, "") == 0) return INV_DT;
    if(strlen(a) != 2) return INV_DT;

    double month = 0;
    month = strtol(a, NULL, 10);

    if(month < 1 || month > 12) return INV_DT;

    return OK;
}

VCardErrorCode dayCheck(char *a) {
    if(a == NULL) return INV_DT;
    if(strcmp(a, "") == 0) return INV_DT;
    if(strlen(a) != 2) return INV_DT;

    double month = 0;
    month = strtol(a, NULL, 10);

    if(month < 1 || month > 31) return INV_DT;

    return OK;
}

void countCardinality(Property *prop, List *instances) {
    ListIterator instanceIter;
    void *node;
    Counter *currProp;
    Counter *toBeAdded;
    char *tmpName;

    instanceIter = createIterator(instances);
    while((node = nextElement(&instanceIter)) != NULL) {
        currProp = (Counter *)node;
        tmpName = malloc(sizeof(char) * (strlen(prop->name) + 1));
        strcpy(tmpName, prop->name);
        strcat(tmpName, "\0");
        if(strcmp(currProp->propName, upperCaseStr(tmpName)) == 0) {
            currProp->count++;
            free(tmpName);
            return;
        }
        free(tmpName);
    }

    toBeAdded = malloc(sizeof(Counter) + sizeof(char) * (strlen(prop->name) + 1));
    strcpy( toBeAdded->propName, upperCaseStr(prop->name));
    strcat(toBeAdded->propName, "\0");
    toBeAdded->count = 1;
    insertBack(instances, toBeAdded);
}

char *printCounter(void *toBePrinted) {
    return NULL;
}
void deleteCounter(void *toBeDeleted) {

    if(toBeDeleted == NULL) return;

    Counter *tmp;
    tmp = (Counter *)toBeDeleted;

    free(tmp);

    return;
}

int compareCounter(const void *first, const void *second) {
    if(first == NULL) return -1;
    if(second == NULL) return -1;

    Counter *a = (Counter *)first;
    Counter *b = (Counter *)second;

    return strcmp(a->propName, b->propName);
}

VCardErrorCode checkCardinality(List *instances) {
    ListIterator instanceIter;
    void *node;
    Counter *currProp;

    instanceIter = createIterator(instances);
    while((node = nextElement(&instanceIter)) != NULL) {
        currProp = (Counter *)node;
        if(strcmp(currProp->propName, "KIND\0") == 0) {
            if(currProp->count > 1) return INV_PROP;
        }
        else if(strcmp(currProp->propName, "FN\0") == 0) {
            if(currProp->count < 0) return INV_PROP;
        }
        else if(strcmp(currProp->propName, "N\0") == 0) {
            if(currProp->count > 1) return INV_PROP;
        }
        else if(strcmp(currProp->propName, "GENDER\0") == 0) {
            if(currProp->count > 1) return INV_PROP;
        }
        else if(strcmp(currProp->propName, "PRODID\0") == 0) {
            if(currProp->count > 1) return INV_PROP;
        }
        else if(strcmp(currProp->propName, "REV\0") == 0 ) {
            if(currProp->count > 1) return INV_PROP;
        }
    }
    return OK;
}

VCardErrorCode validateCard(const Card* obj) {
    
    if(obj == NULL) return INV_CARD;
    if(obj->fn == NULL) return INV_CARD;
    if(obj->optionalProperties == NULL) return INV_CARD;

    VCardErrorCode retVal;
    ListIterator optionalPropIter;
    void *node;
    Property *currProp;
    List *instances = initializeList(&printCounter, &deleteCounter, &compareCounter);

    //validate struct implementation constraints and vCard Specifications
    retVal = checkPropStruct(obj->fn);
    if(retVal != OK) {
        freeList(instances);
        return retVal;
    }
    optionalPropIter = createIterator(obj->optionalProperties);
    while((node = nextElement(&optionalPropIter)) != NULL) {
        currProp = (Property *)node;
        retVal = checkPropStruct(currProp);
        if(retVal != OK) {
            freeList(instances);
            return retVal;
        }
        countCardinality(currProp, instances);
    }
    retVal = checkDTStruct(obj->birthday);
    if(retVal != OK) {
        freeList(instances);
        return retVal;
    }

    retVal = checkDTStruct(obj->anniversary);
    if(retVal != OK) {
        freeList(instances);
        return retVal;
    }

    retVal = checkCardinality(instances);
    if(retVal != OK) {
        freeList(instances);
        return retVal;
    }
    
    freeList(instances);
    return OK;
}

//A1 functions
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
                i -= 3;
                fileLen -= 3;
            }

            if(buff[i] == '\\') {
                i -= 1;
                fileLen -= 1;
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

    if(beginBuff(buffer) != OK && CREATECARD_ERROR_CHECKING) {
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
    while(token != NULL && strncmp(token, "END:VCARD", 10) != 0) {
        if(strlen(token) > 998 && CREATECARD_ERROR_CHECKING) {
            free(buffCpy);
            return INV_PROP;
        }
        //get group
        groupLen = 0;
        lGroup = getGroup(token);
        if(lGroup != NULL) {
            if(DEBUG) {printf("\nGroup: %s\n", lGroup);}
            retVal = checkGroup(lGroup);
            if(retVal != OK && CREATECARD_ERROR_CHECKING) {
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
        if(retVal != OK && CREATECARD_ERROR_CHECKING) {
            freeLine(&lGroup, &lProp, &lParam, &lVal);
            free(buffCpy);
            return retVal;
        }

        //get parameter
        lParam = getParam(token);
        if(lParam != NULL) {
            if(DEBUG) {printf("Param: %s\n", lParam);}
            retVal = checkParam(lParam);
            if(retVal != OK && CREATECARD_ERROR_CHECKING) {
                freeLine(&lGroup, &lProp, &lParam, &lVal);
                free(buffCpy);
                return retVal;
            }
        }

        //get value
        lVal = getValue(token);
        retVal = checkValue(lVal);
        if(DEBUG) {printf("Value: %s\n", lVal);}
        if(retVal != OK && CREATECARD_ERROR_CHECKING) {
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
        else if( (strcmp(upperCaseStr(lProp), "BDAY") == 0 && ((*newCardObject)->birthday == NULL)) || ( ((*newCardObject)->anniversary == NULL ) && strcmp(upperCaseStr(lProp), "ANNIVERSARY") == 0 ) )  {
            deleteProperty(newProp);
            newDate = malloc( sizeof(DateTime) + ( (strlen(lVal) + 1) *sizeof(char) ) );
            if(strstr(lParam, "VALUE=text") == NULL) {
                newDate->isText = false;
                strcpy(newDate->text, "\0");
                if(lVal[strlen(lVal) - 1] == 'Z') {
                    newDate->UTC = true;
                    lVal[strlen(lVal) - 1] = '\0';
                }
                else {
                    newDate->UTC = false;
                }

                tPos = myStrChr(lVal, 'T');
                if(tPos == NULL) {
                    strcpy(newDate->date, lVal);
                    strcpy(newDate->time, "\0");
                }

                else if(tPos == lVal) {
                    strncpy(newDate->time, lVal + 1, strlen(lVal)- 1);
                    strcpy(newDate->date, "\0");
                }
                else {
                    strncpy(newDate->date, lVal, tPos-lVal + 1);
                    newDate->date[tPos-lVal] = '\0';
                    strncpy(newDate->time, tPos + 1, 7);
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

            if(newDate->isText == false) {
                if(dateCheck(lVal) != OK && CREATECARD_ERROR_CHECKING) {
                    freeLine(&lGroup, &lProp, &lParam, &lVal);
                    deleteDate(newDate);
                    free(buffCpy);
                    return INV_PROP;
                }
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

    if(endBuff(buffer) != OK && CREATECARD_ERROR_CHECKING) {
        if(DEBUG) {printf("Card could not be created. Incorrect/missing end.\n");}
        free(buffCpy);
        return INV_CARD;
    }

    if(numContentLines == 0 && CREATECARD_ERROR_CHECKING) {
        if(DEBUG) {printf("Card could not be created. No Content Lines.\n");}
        free(buffCpy);
        return INV_CARD;
    }

    if((*newCardObject)->fn == NULL && CREATECARD_ERROR_CHECKING) {
        if(DEBUG) {printf("Card could not be created. No FN property.\n");}
        free(buffCpy);
        return INV_CARD;
    }
    free(buffCpy);
    return OK;
}

VCardErrorCode dateCheck(char *a) {
    int flag;

    if(a == NULL) {
        return INV_PROP;
    }
    
    flag = 0;

    for(int i = 0; a[i] != '\0'; i++) {
        if(a[strlen(a) - 1] == 'Z') {
            break;
        }
        if(isdigit(a[i]) == 0 && a[i] != '-') {
            if(a[i] == 'T' && flag == 0) {
                flag = 1;
                continue;
            }
            else {
                return INV_PROP;
            }
        }
    }

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
        endPos = myStrChr(lParam, ';');
        currPos = lParam;
        while(endPos != NULL) {
            currParam = malloc(sizeof(char) * (endPos - currPos + 1));
            strncpy(currParam, currPos, endPos - currPos);
            currParam[endPos - currPos] = '\0';
            insertParam(parList, currParam);
            free(currParam);
            currPos = endPos + 1;
            endPos = NULL;
            endPos = myStrChr(currPos, ';');
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
        endPos = myStrChr(lVal, ';');
        currPos = lVal;
        while(endPos != NULL) {
            currVal = malloc(sizeof(char) * (endPos - currPos + 1));
            strncpy(currVal, currPos, endPos - currPos);
            currVal[endPos - currPos] = '\0';
            insertValue(valList, currVal);
            free(currVal);
            currPos = endPos + 1;
            endPos = NULL;
            endPos = myStrChr(currPos, ';');
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

    equalSign = myStrChr(par, '=');
    end = myStrChr(par, '\0');

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
    
    if(strlen(lProp) < 1) return INV_PROP;

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
	char *equalSign = NULL;
	if(lParam == NULL) return OK;
	if(strlen(lParam) == 0) return OK;
	equalSign = myStrChr(lParam, '=');
	if(equalSign == NULL) return INV_PROP;
	if(strlen(equalSign + 1) < 1) return INV_PROP;
    return OK;
}

VCardErrorCode checkValue(char *lVal) {
	if(lVal == NULL) return INV_PROP;
	if(strlen(lVal) < 1) return INV_PROP;
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

    for(int i = 0; i < 26; i++) {
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

    val = myStrChr(token, ':');
    group = myStrChr(token, '.');
    sc = myStrChr(token, ';');

    if(sc != NULL) {
        if(val == NULL || group == NULL || group > val || group > sc) {
            toReturn = malloc(sizeof(char));
            strcpy(toReturn, "\0");
            return toReturn;
        }
    }
    else {
        if(val == NULL || group == NULL || group > val) {
            toReturn = malloc(sizeof(char));
            strcpy(toReturn, "\0");
            return toReturn;
        }
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

    val = myStrChr(token, ':');
    group = myStrChr(token, '.');
    sc = myStrChr(token, ';');

    if(val == NULL) {
        return NULL;
    }

    if(sc != NULL) {
        if(group != NULL && group < val && group < sc) {
            toReturn = malloc(sizeof(char) * (val - group + 1));
            strncpy(toReturn, group + 1, val - group - 1);
            toReturn[val - group - 1] = '\0';
            if(numSemiColons(toReturn) != 0) {
                sc = myStrChr(toReturn, ';');
                *sc = '\0';
            }
            return toReturn;
        }
        else {
            toReturn = malloc(sizeof(char) * (val - token + 1));
            strncpy(toReturn, token, val - token);
            toReturn[val - token] = '\0';
            if(numSemiColons(toReturn) != 0) {
                sc = myStrChr(toReturn, ';');
                *sc = '\0';
            }
            return toReturn;
        }
    }
    else {
        if(group != NULL && group < val) {
            toReturn = malloc(sizeof(char) * (val - group + 1));
            strncpy(toReturn, group + 1, val - group - 1);
            toReturn[val - group - 1] = '\0';
            if(numSemiColons(toReturn) != 0) {
                sc = myStrChr(toReturn, ';');
                *sc = '\0';
            }
            return toReturn;
        }
        else {
            toReturn = malloc(sizeof(char) * (val - token + 1));
            strncpy(toReturn, token, val - token);
            toReturn[val - token] = '\0';
            if(numSemiColons(toReturn) != 0) {
                sc = myStrChr(toReturn, ';');
                *sc = '\0';
            }
            return toReturn;
        }
    }
}

char *getParam(char *token) {
    char *toReturn = NULL;
    char *group = NULL;
    char *val = NULL;
    char *sc = NULL;

    val = myStrChr(token, ':');
    group = myStrChr(token, '.');
    sc = myStrChr(token, ';');

    if(group != NULL && group < val && group < sc) {
        toReturn = malloc(sizeof(char) * (val - group + 1));
        strncpy(toReturn, group, val - group);
        toReturn[val - group] = '\0';
        if(numSemiColons(toReturn) == 0) {
            strcpy(toReturn, "");
            return toReturn;
        }
        else {
            sc = myStrChr(toReturn, ';');
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
            sc = myStrChr(toReturn, ';');
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

    val = myStrChr(token, ':');
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
    if(fileCheck(fileName, fp) != OK && CREATECARD_ERROR_CHECKING) {
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
    toReturn = malloc( (sizeof(char) * (strlen(str) + 2)));
    strcpy(toReturn, str);
    strcat(toReturn, "\n");
    strcat(toReturn, "\0");
    free(str);

    propIter = createIterator(obj->optionalProperties);
    while((node = nextElement(&propIter)) != NULL) {
        tmp = (Property *)node;
        str = printProperty(tmp);
        toReturn = realloc(toReturn, (sizeof(char) * (strlen(toReturn) + strlen(str) + 2)));
        strcat(toReturn, str);
        strcat(toReturn, "\n");
        strcat(toReturn, "\0");
        free(str);
    }

    str = printDate(obj->birthday);
    toReturn = realloc(toReturn, (sizeof(char) * (strlen(toReturn) + strlen(str) + 8)));
    strcat(toReturn, "BDAY:");
    strcat(toReturn, str);
    strcat(toReturn, "\n");
    strcat(toReturn, "\0");
    free(str);

    str = printDate(obj->anniversary);
    toReturn = realloc(toReturn, (sizeof(char) * (strlen(toReturn) + strlen(str) + 15)));
    strcat(toReturn, "ANNIVERSARY:");
    strcat(toReturn, str);
    strcat(toReturn, "\n");
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
            toReturn = malloc(sizeof(char) * (strlen("Invalid Date-Time\0") + 1));
            strcpy(toReturn, "Invalid Date-Time\0");
            break;
        case 5:
            toReturn = malloc(sizeof(char) * (strlen("Write Error\0")+1));
            strcpy(toReturn, "Write Error\0");
            break;
        case 6:
            toReturn = malloc(sizeof(char) * (strlen("Other Error\0")+1));
            strcpy(toReturn, "Other Error\0");
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
    if(strlen(a->group) > 0) strcat(toReturn, ".");
    strcat(toReturn, a->name);
    if(getLength(a->parameters) == 0) strcat(toReturn, ":");
    strcat(toReturn, "\0");

    paramIter = createIterator(a->parameters);
    valueIter = createIterator(a->values);

    while((node = nextElement(&paramIter)) != NULL) {
        Parameter *tmp;
        tmp = (Parameter *)node;
        
        str = a->parameters->printData(tmp);
        toReturn = realloc(toReturn, strlen(toReturn) + strlen(str) + 2);
        strcat(toReturn, str);
        strcat(toReturn, "\0");
        free(str);
    }
    if(getLength(a->parameters) != 0) {
        strcat(toReturn, ":");
        strcat(toReturn, "\0");
    }

    while((node = nextElement(&valueIter)) != NULL) {
        char *tmp;
        tmp = (char *)node;

        str = a->values->printData(tmp);
        toReturn = realloc(toReturn, strlen(toReturn) + strlen(str) + 2);
        strcat(toReturn, str);
        strcat(toReturn, ";");
        strcat(toReturn, "\0");
        free(str);
    }
    toReturn[strlen(toReturn) - 1] = '\0';
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

    toReturn = malloc(sizeof(char) * (203 + strlen(a->value)));

    strcpy(toReturn, ";");
    strcat(toReturn, a->name);
    strcat(toReturn, "=");
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
    char *castedArg;
    if(toBePrinted == NULL) {
        a = malloc(sizeof(char));
        strcpy(a, "\0");
        return a;
    }

    castedArg = (char *)toBePrinted;
    a = malloc(sizeof(char) * (strlen(castedArg) + 1));
    strcpy(a, castedArg);
    strcat(a,"\0");
    
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
        if(strlen(a->time) != 0 ) strcat(toReturn, "T");
        strcat(toReturn, a->time);
        if(a->UTC == true) strcat(toReturn, "Z");
        strcat(toReturn, "\0");
    }
    return toReturn;
}
