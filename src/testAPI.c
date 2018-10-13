#include "VCardParser.h"
#define CTEST 0
#define PTEST 0
#define WTEST 1

int main(int argc, char **argv) {
    Card *testCardOne;
    VCardErrorCode retVal;
    char *retString;
    char *printedCard = NULL;

    //createCard Test
    retVal = createCard(argv[argc - 1], &testCardOne);
    retString = printError(retVal);
    if(CTEST) printf("createCard Status: %s\n", retString);
    free(retString);
    retString = NULL;

    //printCard Test
    printedCard = printCard(testCardOne);
    if(printedCard != NULL) {
        if(PTEST) printf("%s", printedCard);
        free(printedCard);
    }

    //writeCard Test
    retVal = writeCard("writeOne.vcf", testCardOne);
    retString = printError(retVal);
    if(WTEST) printf("writeCard Status: %s\n", retString);
    free(retString);
    retString = NULL;
    deleteCard(testCardOne);
    return 0;
}