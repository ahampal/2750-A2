#include "VCardParser.h"
#define CTEST 1
#define PTEST 0
#define WTEST 1

int main(int argc, char **argv) {
    Card *testCardOne;
    VCardErrorCode retVal;
    char *retString;

    //createCard Test
    if(CTEST) {
        retVal = createCard(argv[argc - 1], &testCardOne);
        retString = printError(retVal);
        printf("createCard Status: %s\n", retString);
        free(retString);
        retString = NULL;
    }

    //printCard Test
    if(PTEST) {
        char *printedCard = NULL;
        printedCard = printCard(testCardOne);
        if(printedCard != NULL) {
            printf("%s\n", printedCard);
            free(printedCard);
            free(printedCard);
        }
    }

    //writeCard Test
    if(WTEST) {
        retVal = writeCard("writeOne.vcf", testCardOne);
        retString = printError(retVal);
        printf("writeCard Status: %s\n", retString);
        free(retString);
        retString = NULL;
        deleteCard(testCardOne);
    }
    return 0;
}