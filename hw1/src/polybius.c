#include "polybius.h"


void createPolybiusTable(int row, int col){

    if(key != NULL){
        //printf("KEY: %s\n", key);
        printf("Before: \n");
        display(polybius_table, row, col);
        populateTable(key, row, col);
        printf("After: \n");
        display(polybius_table, row, col);
    }
    printf("After 2: \n");
    populateTable(polybius_alphabet, row, col);
    display(polybius_table, row, col);
}

int encryptP(char ch){
    return (int) ch;
}

char decryptP(int pos){
    return (char) pos;
}

void populateTable(const char* words, int row, int col){
    int count_insert = 0;
    for(int i = 0; i < row*col && *(words + count_insert)!='\0' && count_insert < myStrLen((char*)words); i++){
        char candidate =*(words+count_insert);
        if(myStrContains(polybius_table, candidate) != NULL){
            count_insert++;
            i--;
            continue;
        }
        if(*(polybius_table + i) == '\0') {
            *(polybius_table + i) = candidate;
            count_insert++;
        }
    }

}