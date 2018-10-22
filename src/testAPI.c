#include "CardTestUtilities.h"

#define CTEST 1
#define PTEST 1
#define WTEST 1
#define VTEST 1
#define STEST 1
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

    //createCard Test
    retVal = createCard(argv[argc - 1], &refCard);
    retString = printError(retVal);
    if(CTEST) {
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
        if(PTEST) printf(BLUE"\nTESTING PRINTCARD\n"RESET GRN"%s"RESET, printedCard);
        free(printedCard);
    }
    else {
        printf(BLUE"\nTESTING PRINTCARD\n"RESET RED"Attempted to print null card.\n"RESET);
        free(printedCard);
    }

    //writeCard Test
    retVal = writeCard("writeOne.vcf", refCard);
    retString = printError(retVal);
    if(WTEST) {
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

    if(VTEST) {
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
    if(STEST) {
        printf(BLUE"\nTESTING STRLISTTOJSON\n"RESET);
        optionalPropIter = createIterator(refCard->optionalProperties);
        while(( tmp = nextElement(&optionalPropIter) ) != NULL) {
            prop = (Property *)tmp;
            listToStr = strListToJSON(prop->values);
            if(listToStr == NULL) {
                printf(RED"Null JSON String\n"RESET);
            }
            else {
                printf(GRN"JSON String is: %s\n"RESET, listToStr);
            }
            free(listToStr);
            listToStr = NULL;
        }
    }

    deleteCard(refCard);
    return 0;
}