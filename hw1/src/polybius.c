#include "polybius.h"


int p_row = 10, p_col = 10;

void createPolybiusTable(int l_row, int l_col){
    p_row = l_row;
    p_col = l_col;

    if(key != NULL){
        //printf("KEY: %s\n", key);
        //printf("Before: \n");
        //display(polybius_table, p_row, p_col);
        populateTable(key);
        //printf("After: \n");
        //display(polybius_table, p_row, p_col);
    }
    printf("After 2: \n");
    populateTable(polybius_alphabet);
    display(polybius_table, p_row, p_col);
}

bool encryptP(char ch){
    int r = findRow(ch);
    int c = findCol(ch);
    if(r == -1 || c == -1) return false;

    printf("%X", r);
    printf("%X", c);

    return true;
}

char decryptP(int pos){
    return (char) pos;
}

void populateTable(const char* words){
    int count_insert = 0;
    for(int i = 0; i < p_row*p_col && *(words + count_insert)!='\0' && count_insert < myStrLen((char*)words); i++){
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

int findRow(char ch){
    int pos = -1;

    for(int i = 0; i<myStrLen(polybius_table) && *(polybius_table+i)!='\0'; i++){
        if(*(polybius_table+i) == ch){
            pos = i/ p_col;
        }
    }
    return pos;
}

int findCol(char ch){
    int pos = -1;

    for(int i = 0; i<myStrLen(polybius_table) && *(polybius_table+i)!='\0'; i++){
        if(*(polybius_table+i) == ch){
            pos = i%p_col;
        }
    }

    return pos;
}