#include <stdio.h>
#include <stdlib.h>

unsigned long FILE_SIZE = 2715763;

char* getWordList(char* fileName) {
    FILE* file = fopen(fileName, "r");
    char* buf = malloc(FILE_SIZE);
    unsigned long readSize = fread(buf, FILE_SIZE, 1, file);
    if (readSize == FILE_SIZE) {
        printf("Wordlist read in correctly\n");
    } else {
        printf("Error reading in wordlist, read %lu/2715763 bytes\n", readSize);
    }
    fclose(file);
    return buf;
}

int main() {
    char* wordlist = getWordList("wordlist.txt");

    fwrite(&wordlist[2715750], 12, 1, stdout);

    free(wordlist);
    return 0;
}
