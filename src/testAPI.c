#include "VCardParser.h"

int main(int argc, char **argv) {
    Card *testCardOne = NULL;
    char *printedCard = NULL;
    VCardErrorCode retVal;
    retVal = createCard(argv[argc - 1], &testCardOne);
    printf("result: %d\n", retVal);
    printedCard = printCard(testCardOne);
    if(printedCard != NULL) {
        printf("%s\n", printedCard);
        free(printedCard);
    }
    deleteCard(testCardOne);
    return 0;
}