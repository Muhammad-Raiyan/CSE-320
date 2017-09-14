#include "fractionated.h"
long space = 0;
char *mybuffer_head =(char*) &space;
char *mybuffer_tail =(char*) &space;

void makeFractionatedKey(const char* key){
    //printf("%d\n", myStrLen(key));
    int i = 0;
    for(; *(key+i) != '\0' || i<myStrLen(key); i++){
        *(fm_key + i) = *(key+i);
        //printf("%d: %c ", i, *(key+i));
    }
    //printArr(fm_key, 27);

    for(int j=0; j < myStrLen(fm_alphabet); j++){
        char cand = *(fm_alphabet + j);
        if(myStrContains(fm_key, cand) == NULL){
            *(fm_key+i) = *(fm_alphabet+j);
            i++;
        } else{
            continue;
        }
    }
    //printArr(fm_key, 27);
}

bool encryptF(char ch){
    printf("In encryptF\n");
    int charIndOnMT = ch - '!';
    printf("Index: %d\n", charIndOnMT);
    textToMorseCode(charIndOnMT);
    return true;
}

bool decryptF(char ch){
    return true;
}

void textToMorseCode(int indexOnMorseTalbe){
    printf("%d\n", myStrLen(mybuffer_head));
    mybuffer_head = ((char *)gimorse_table + indexOnMorseTalbe);
    mybuffer_tail++;
    printf("%d\n", myStrLen(mybuffer_head));
    //return fm_key;
}

char* cipherMorseCode(char * ch){
    return NULL;
}