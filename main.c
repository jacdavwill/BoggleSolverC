#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define IN_WORD 0
#define OUT_WORD 1
#define ROW 4
#define MAX_STR_LEN 17 // meaning max word length is 16
unsigned long FILE_SIZE = 2715763;

char* board = "serspatglinesers";
struct Node* baseNode;
//int wordCount = 0;

struct Node {
    long value; // the number of words that that are farther down this branch
    char found;
    char word[MAX_STR_LEN]; // the word that ends on this node
    struct Node* childNodes[26];
};

double getTimeDiff(struct timeval start, struct timeval end) {
    return  (double)(1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)) / 1000;
}

void free_all_trie(struct Node* node) {
    if(node == NULL) return;
    for (int i = 0; i < 26; i++) {
        free_all_trie(node->childNodes[i]);
    }
    free(node);
}

char* getWordList(char* fileName) {
    FILE* file = fopen(fileName, "r");
    char* buf = malloc(FILE_SIZE);
    size_t readSize = fread(buf, 1, FILE_SIZE, file);
    if (readSize == FILE_SIZE) {
        printf("Successfully read %lu chars from file\n", readSize);
    } else {
        printf("Error reading in wordlist, read %lu/2715763 bytes\n", readSize);
    }
    fclose(file);
    return buf;
}

int adj(const int pos, int* places) {
    int x = pos / ROW;
    int y = pos % ROW;
    if (x == 0 && y == 0) {
        places[0] = pos + 1;
        places[1] = pos + ROW;
        places[2] = pos + ROW + 1;
        return 3;
    } else if (x == 0 && y == ROW - 1) {
        places[0] = pos - 1;
        places[1] = pos + ROW;
        places[2] = pos + ROW - 1;
        return 3;
    } else if (x == ROW - 1 && y == 0) {
        places[0] = pos + 1;
        places[1] = pos - ROW;
        places[2] = pos - ROW + 1;
        return 3;
    } else if (x == ROW - 1 && y == ROW - 1) {
        places[0] = pos - 1;
        places[1] = pos - ROW;
        places[2] = pos - ROW - 1;
        return 3;
    } else if (x == 0) {
        places[0] = pos - 1;
        places[1] = pos + 1;
        places[2] = pos + ROW;
        places[3] = pos + ROW - 1;
        places[4] = pos + ROW + 1;
        return 5;
    } else if (x == ROW - 1) {
        places[0] = pos - 1;
        places[1] = pos + 1;
        places[2] = pos - ROW;
        places[3] = pos - ROW - 1;
        places[4] = pos - ROW + 1;
        return 5;
    } else if (y == 0) {
        places[0] = pos + ROW;
        places[1] = pos - ROW;
        places[2] = pos + ROW + 1;
        places[3] = pos + 1;
        places[4] = pos - ROW + 1;
        return 5;
    } else if (y == ROW - 1) {
        places[0] = pos + ROW;
        places[1] = pos - ROW;
        places[2] = pos + ROW - 1;
        places[3] = pos - 1;
        places[4] = pos - ROW - 1;
        return 5;
    } else {
        places[0] = pos + 1;
        places[1] = pos - 1;
        places[2] = pos + ROW;
        places[3] = pos + ROW - 1;
        places[4] = pos + ROW + 1;
        places[5] = pos - ROW;
        places[6] = pos - ROW - 1;
        places[7] = pos - ROW + 1;
        return 8;
    }
}

int contains(const int* list, const int listLastPos, const int target) {
    for (int i = 0; i <= listLastPos; i++) {
        if (list[i] == target) {
            return 1;
        }
    }
    return 0;
}

void search(int hist[], int histLastPos, struct Node* node) {
    int adjPos[8];
    for (int i = 0; i < adj(hist[histLastPos], adjPos); i++) {
        int pos = adjPos[i];
        if (!contains(hist, histLastPos, pos)) {
            struct Node* next = node->childNodes[board[pos] - 'a'];
            if (next != NULL) {
                hist[histLastPos + 1] = pos;
                if (next->word[0] != 0) {
                    struct Node* temp;
                    temp = baseNode;
                    for (int j = 0; j < strlen(next->word); j++) {
                        temp = temp->childNodes[next->word[j] - 'a'];
                    }
                    temp->found = 1;
                }
                if (next->value > 0) {
                    int histCopy[MAX_STR_LEN - 1];
                    memcpy(histCopy, hist, (histLastPos + 2) * sizeof(int));
                    search(histCopy, histLastPos + 1, next);
                }
            }
        }
    }
}

int main() {
    struct timeval startProg, endProcess, endSearch, endProg;

    gettimeofday(&startProg, NULL);
    char* wordlist = getWordList("wordlist.txt");
    baseNode = malloc(sizeof(struct Node));
    memset(baseNode, 0, sizeof(struct Node));
    long wordStart = 0;
    int state = OUT_WORD;
    long pos = -1;
    struct Node* currNode = baseNode;
    char letter;
    while ((letter = wordlist[++pos]) != EOF) {
        if (letter >= 'a' && letter <= 'z') {
            if (pos - wordStart < MAX_STR_LEN) {
                if (state == OUT_WORD) {
                    wordStart = pos;
                    state = IN_WORD;
                }
                int slot = letter - 'a';
                if (currNode->childNodes[slot] == NULL) {
                    currNode->childNodes[slot] = malloc(sizeof(struct Node));
                    memset(currNode->childNodes[slot], 0, sizeof(struct Node));
                }
                currNode->value++;
                currNode = currNode->childNodes[slot];
            }
        } else {
            if (state == IN_WORD) {
                long wordLen = pos - wordStart;
                if (wordLen > 2 && wordLen < MAX_STR_LEN) {
                    memcpy(currNode->word, &wordlist[wordStart], wordLen);
                }
                wordStart = pos;
                currNode = baseNode;
                state = OUT_WORD;
            }
        }
    }
    gettimeofday(&endProcess, NULL);

    for (int i = 0; i < strlen(board); i++) {
        int hist[MAX_STR_LEN - 1] = { 0 }; // to remove next line change this line to int hist[MAX_STR_LEN] = { i };
        hist[0] = i;
        search(hist, 0, baseNode->childNodes[board[i] - 'a']);
    }
    gettimeofday(&endSearch, NULL);

    printf("Pre-processing time: %f ms\n", getTimeDiff(startProg, endProcess));
    printf("Search time: %f ms\n", getTimeDiff(endProcess, endSearch));
    free(wordlist); // word count: 259,709
    free_all_trie(baseNode);
    return 0;
}
