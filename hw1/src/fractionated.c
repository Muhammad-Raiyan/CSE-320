#include "fractionated.h"

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
    /*for(int j=0; *(fm_alphabet+j)!= '\0'; j++){
        char cand = *(fm_alphabet+j);
        if(myStrContains(fm_key, cand)!=NULL){
            continue;
        } else {
            if((fm_key+last_insert)==NULL){
                *(fm_key+last_insert) = *(fm_alphabet+j);
            }
            else {
                j --;
            }
            last_insert++;
        }
    }*/
    printArr(fm_key, 27);
}

bool encryptF(char ch){
    return true;
}

bool decryptF(char ch){
    return true;
}

char* textToMorseCode(char ch){
    return fm_key;
}

char* cipherMorseCode(char * ch){
    return NULL;
}