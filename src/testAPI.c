#include "VCardParser.h"

int main(int argc, char **argv) {
    Card *testCardOne = NULL;
    char *printedCard = NULL;
    VCardErrorCode retVal;
    char *retString;

    retVal = createCard(argv[argc - 1], &testCardOne);
    retString = printError(retVal);
    printf("%s\n", retString);
    free(retString);
    printedCard = printCard(testCardOne);
    if(printedCard != NULL) {
        printf("%s\n", printedCard);
        free(printedCard);
    }
    deleteCard(testCardOne);
    return 0;
}