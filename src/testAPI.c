#include "CardTestUtilities.h"

#define CREATE_CARD_TEST 1
#define PRINT_CARD_TEST 1
#define WRITE_CARD_TEST 1
#define VALIDATE_CARD_TEST 1
#define STRLISTTOJSON_TEST 1
#define JSONTOSTRLIST_TEST 1
#define PROPTOJSON_TEST 1
#define JSONTOPROP_TEST 1
#define DTTOJSON_TEST 1
#define ADDPROP_TEST 1
#define JSONTOCARD_TEST 1
#define NUM_INV_CARDS 31
#define NUM_V_CARDS 12
#define BLUE "\033[1m\033[34m"

int main(int argc, char **argv) {
    Card *refCard;
    Card *writeOne;
    VCardErrorCode retVal;
    char *retString;
    char *printedCard = NULL;
    Card *invCard;
    char *invCardDir;
    char *listToStr;
    ListIterator optionalPropIter;
    Property *prop;
    void *tmp;
    List *tmpList;
    char *jsonString;
    Property *AnotherProp;
    DateTime *tmpDate;
    Card *jsonCard;
    int flag;

    //createCard Test
    retVal = createCard(argv[argc - 1], &refCard);
    retString = printError(retVal);
    if(CREATE_CARD_TEST) {
        printf(BLUE"TESTING CREATECARD\n"RESET);
        if(retVal == OK) {
            printf(GRN);
        }
        else {
            printf(RED);
        }
        printf("createCard Status: %s\n", retString);
        printf(RESET);
    }
    free(retString);
    retString = NULL;

    //printCard Test
    printedCard = printCard(refCard);
    if(printedCard != NULL) {
        if(PRINT_CARD_TEST) printf(BLUE"\nTESTING PRINTCARD\n"RESET GRN"%s"RESET, printedCard);
        free(printedCard);
    }
    else {
        printf(BLUE"\nTESTING PRINTCARD\n"RESET RED"Attempted to print null card.\n"RESET);
        free(printedCard);
    }

    //writeCard Test
    retVal = writeCard("writeOne.vcf", refCard);
    retString = printError(retVal);
    if(WRITE_CARD_TEST) {
        printf(BLUE"\nTESTING WRITECARD\n"RESET);
        if(retVal == OK) {
            printf(GRN);
        }
        else {
            printf(RED);
        }
        printf("writeCard Status: %s\n", retString);
        printf(RESET);
        retVal = createCard("writeOne.vcf", &writeOne);
        if(retVal != OK) printf(RED"writeOne createCard status: %d\n"RESET, retVal);
        else
        if(_tObjEqual(refCard,writeOne)) {
            printf(GRN);
            printf("writeOne.vcf and %s are the same file\n", argv[argc-1]);
        }
        else {
            printf(RED);
            printf("writeOne.vcf and %s are not the same file\n", argv[argc-1]);
        }
        printf(RESET);
        deleteCard(writeOne);
    }
    free(retString);
    retString = NULL;

    if(VALIDATE_CARD_TEST) {
        printf(BLUE"\nTESTING VALIDATECARD WITH INVALID FILES\n"RESET);
        for(int i = 0; i < NUM_INV_CARDS; i++) {
            invCardDir = malloc(sizeof(char) * (strlen("../test_files/validateCardTesting/invalid/test1.vcf") + 4));
            sprintf(invCardDir, "../test_files/validateCardTesting/invalid/test%d.vcf", i);
            retVal = createCard(invCardDir, &invCard);
            retVal = validateCard(invCard);
            if(retVal != OK) {
                retString = printError(retVal);
                printf(GRN"test %d status: %s\n"RESET, i, retString);
                free(retString);
                retString = NULL;
            }
            else {
                retString = printError(retVal);
                printf(RED"test %d status: %s\n"RESET, i, retString);
                free(retString);
                retString = NULL;
            }
            free(invCardDir);
            invCardDir = NULL;
            deleteCard(invCard);
        }
        printf(BLUE"\nTESTING VALIDATECARD WITH VALID FILES\n"RESET);
        for(int i = 0; i < NUM_V_CARDS; i++) {
            invCardDir = malloc(sizeof(char) * (strlen("../test_files/validateCardTesting/valid/test1.vcf") + 4));
            sprintf(invCardDir, "../test_files/validateCardTesting/valid/test%d.vcf", i);
            retVal = createCard(invCardDir, &invCard);
            retVal = validateCard(invCard);
            if(retVal == OK) {
                retString = printError(retVal);
                printf(GRN"test %d status: %s\n"RESET, i, retString);
                free(retString);
                retString = NULL;
            }
            else {
                retString = printError(retVal);
                printf(RED"test %d status: %s\n"RESET, i, retString);
                free(retString);
                retString = NULL;
            }
            free(invCardDir);
            invCardDir = NULL;
            deleteCard(invCard); 
        }
    }

    //strListToJSON testing
    if(STRLISTTOJSON_TEST) {
        printf(BLUE"\nTESTING STRLISTTOJSON\n"RESET);
        optionalPropIter = createIterator(refCard->optionalProperties);
        while(( tmp = nextElement(&optionalPropIter) ) != NULL) {
            prop = (Property *)tmp;
            listToStr = strListToJSON(prop->values);
            if(listToStr == NULL) {
                printf(RED"JSON String is: NULL\n"RESET);
            }
            else {
                printf(GRN"JSON String is: %s\n"RESET, listToStr);
            }
            free(listToStr);
            listToStr = NULL;
        }
    }

    if(JSONTOSTRLIST_TEST) {
        printf(BLUE"\nTESTING JSONTOSTRLIST\n"RESET);
        prop = getFromFront(refCard->optionalProperties);
        jsonString = strListToJSON(prop->values);
        tmpList = JSONtoStrList(jsonString);
        if(tmpList == NULL) {
            printf(RED"Failed to create list\n"RESET);
        }
        else {
            printf(GRN);
            _tPrintList("Successfully created List. Contents of list are: ", tmpList);
            printf(RESET);
        }
        free(jsonString);
        jsonString = NULL;
        freeList(tmpList);
    }

    if(PROPTOJSON_TEST) {
        printf(BLUE"\nTESTING PROPTOJSON\n"RESET);
        optionalPropIter = createIterator(refCard->optionalProperties);
        while((tmp = nextElement(&optionalPropIter)) != NULL) {
            prop = (Property *)tmp;
            jsonString = propToJSON(prop);
            if(strcmp(jsonString, "") == 0) {
                printf(RED"Empty JSON string\n"RESET);
            }
            else {
                printf(GRN"JSON String: %s\n"RESET, jsonString);
            }
            free(jsonString);
            jsonString = NULL;
        }
    }

    if(JSONTOPROP_TEST) {
        printf(BLUE"\nTESTING JSONTOPROP\n"RESET);
        optionalPropIter = createIterator(refCard->optionalProperties);
        while((tmp = nextElement(&optionalPropIter)) != NULL) {
            prop = (Property *)tmp;
            jsonString = propToJSON(prop);
            AnotherProp = JSONtoProp(jsonString);
            if(_tEqualPropNoParam(prop, AnotherProp)) {
                printf(GRN"refProp ands createdProp are equal\n"RESET);
            }
            else {
                printf(RED"properties are not equal\n"RESET);
            }
            free(jsonString);
            deleteProperty(AnotherProp);
        }
    }

    if(DTTOJSON_TEST) {
        jsonString = NULL;
        printf(BLUE"\nTESTING DTTOJSON\n"RESET);
        jsonString = dtToJSON(refCard->birthday);
        if(jsonString != NULL) {
            printf(GRN"BDAY JSON: %s\n"RESET, jsonString);
        }
        else {
            printf(GRN"BDAY JSON NULL\n"RESET);
        }
        free(jsonString);
        jsonString = NULL;
        jsonString = dtToJSON(refCard->anniversary);
        if(jsonString != NULL) {
            printf(GRN"ANNIVERSARY JSON: %s\n"RESET, jsonString);
        }
        else {
            printf(GRN"ANNIVERSARY JSON NULL\n"RESET);
        }
        printf(BLUE"\nTESTING JSONTODT\n"RESET);
        tmpDate = JSONtoDT(jsonString);
        if(tmpDate == NULL) {
            printf(RED"DateTime struct is null\n"RESET);
        }
        else {
            retString = printDate(tmpDate);
            printf(GRN"DateTime struct is: \n%s\n"RESET, retString);
            free(retString);
            retString = NULL;
        }
        deleteDate(tmpDate);
        tmpDate = NULL;
        free(jsonString);
        jsonString = NULL;

        jsonString = dtToJSON(refCard->birthday);
        tmpDate = JSONtoDT(jsonString);
        if(tmpDate == NULL) {
            printf(RED"DateTime struct is null\n"RESET);
        }
        else {
            retString = printDate(tmpDate);
            printf(GRN"DateTime struct is: \n%s\n"RESET,retString);
            free(retString);
            retString = NULL;
        }
        deleteDate(tmpDate);
        tmpDate = NULL;
        free(jsonString);
        jsonString = NULL;

        tmpDate = JSONtoDT("{\"isText\":true,\"date\":\"\",\"time\":\"\",\"text\":\"circa 1989\",\"isUTC\":\"true\"}");
        if(tmpDate == NULL) {
            printf(RED"DateTime struct is Null\n"RESET);
        }
        else {
            retString = printDate(tmpDate);
            printf(GRN"DateTime struct is: \n%s\n"RESET, retString);
            free(retString);
            retString = NULL;
        }
        deleteDate(tmpDate);
        
        tmpDate = JSONtoDT("{\"isText\":,\"date\":\"19540203\",\"time\":\"123012\",\"text\":\"\",\"isUTC\":\"true\"}");
        if(tmpDate != NULL) {
            printf(RED"Failed\n"RESET);
        }
        else {
            printf(GRN"Passed\n"RESET);
        }
        deleteDate(tmpDate);
    }

    if(ADDPROP_TEST) {
        printf(BLUE"\nTESTING ADDPROP\n"RESET);
        addProperty(refCard, refCard->fn);
        optionalPropIter = createIterator(refCard->optionalProperties);
        flag = 0;
        while((tmp = nextElement(&optionalPropIter)) != NULL) {
            AnotherProp = (Property *)tmp;
            if(_tPropEqual(refCard->fn, AnotherProp)) {
                printf(GRN"Passed\n"RESET);
                flag = 1;
                deleteDataFromList(refCard->optionalProperties, refCard->fn);
                break;
            }
        }
        if(flag == 0) {
            printf(RED"Failed\n"RESET);
        }
    }

    if(JSONTOCARD_TEST) {
        printf(BLUE"\nTESTING JSONTOCARD\n"RESET);
        jsonCard = JSONtoCard("{\"FN\":\"Simon Perreault\"}");
        retString = printCard(jsonCard);
        if(retString == NULL) {
            printf(RED"json card is null\n"RESET);
        }
        else {
            printf(GRN"%s\n"RESET, retString);
            free(retString);
            retString = NULL;
        }
        deleteCard(jsonCard);
    }

    deleteCard(refCard);
    return 0;
}