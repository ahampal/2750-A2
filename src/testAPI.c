#include "VCardParser.h"

int main(int argc, char **argv) {
    Card **testCardOne;
    testCardOne = malloc(sizeof(Card *));
    createCard(argv[argc - 1], testCardOne);
    free(testCardOne);
    return 0;
}