#include "VCardParser.h"

int main(int argc, char **argv) {
    Card *testCardOne = NULL;
    VCardErrorCode retVal;
    retVal = createCard(argv[argc - 1], &testCardOne);
    printf("result: %d\n", retVal);
    deleteCard(testCardOne);
    return 0;
}