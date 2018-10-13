#include "CardTestUtilities.h"

#define CTEST 1
#define PTEST 0
#define WTEST 1
#define VTEST 1

int main(int argc, char **argv) {
    Card *refCard;
    Card *writeOne;
    VCardErrorCode retVal;
    char *retString;
    char *printedCard = NULL;

    //createCard Test
    retVal = createCard(argv[argc - 1], &refCard);
    retString = printError(retVal);
    if(CTEST) {
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
        if(PTEST) printf("%s", printedCard);
        free(printedCard);
    }

    //writeCard Test
    retVal = writeCard("writeOne.vcf", refCard);
    retString = printError(retVal);
    if(WTEST) {
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

    //validateCard Test
    retVal = validateCard(refCard);
    retString = printError(retVal);
    if(VTEST) {
        if(retVal != OK) {
            printf(RED);
        }
        else {
            printf(GRN);
        }
        printf("validateCard Status: %s\n", retString);
        printf(RESET);
    }
    free(retString);
    retString = NULL;

    deleteCard(refCard);
    return 0;
}