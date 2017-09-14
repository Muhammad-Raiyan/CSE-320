#include "fractionated.h"
#include <stdlib.h>

char *mybuffer = polybius_table;
bool alreadyHasWS = false;

void makeFractionatedKey(const char* key){
    //printf("%d\n", myStrLen(key));
    int i = 0;
    if(key!=NULL){
        for(; *(key+i) != '\0' || i<myStrLen(key); i++){
            *(fm_key + i) = *(key+i);
            //printf("%d: %c ", i, *(key+i));
        }
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
    if(alreadyHasWS== true && ch == ' ')
        return encryptF(getchar());
    if(ch == '\n' || ch == '\r') {
        alreadyHasWS = false;
        insertEndOfBuffer('x');
        //printf("%s\n", mybuffer);
        clearArray(mybuffer);
        printf("\n");
        return true;
    }
    if(ch == ' '){
        insertEndOfBuffer('x');
        alreadyHasWS = true;
        return encryptF(getchar());
    }
    int charIndOnMT = ch - '!';
    alreadyHasWS = false;
    //printf("Index: %d\n", charIndOnMT);
    textToMorseCode(charIndOnMT);
    while(isBuffFull()){
        //printf("Buff is Full: %s\n", mybuffer);
        long space = 0;
        char* target = (char*) &space;
        getTriplet(target);
        //printf("REMOVED: %s\n", target);
        printf("%c", cipherMorseCode(target));
        removeFromFront();
    }

    return encryptF(getchar());
}

bool decryptF(char ch){
    return true;
}

void textToMorseCode(int indexOnMorseTable){

    char *tempStr = (char*)*(morse_table + indexOnMorseTable);

    for(int i = 0; *(tempStr+i)!='\0'; i++){
        insertEndOfBuffer(*(tempStr+i));
    }
    insertEndOfBuffer('x');
}

char cipherMorseCode(const char *code){
    for(int i = 0; i<26; i++){
        char* cand = (char*)*(fractionated_table+i);
        if(myStrCmp(cand, code)){
            //printf("Found target\n");
            return *(fm_key+i);
        }
    }
    return '\0';
}

void insertEndOfBuffer(char ch){
    int len = myStrLen(mybuffer);
    *(mybuffer+len) = ch;
}

void removeFromFront(){
    for(int i=0; *(mybuffer+i)!= '\0'; i+=3){
        *(mybuffer+i) = *(mybuffer+i+3);
        *(mybuffer+i+1) = *(mybuffer+i+4);
        *(mybuffer+i+2) = *(mybuffer+i+5);
    }
}

void getTriplet(char* temp_buff){

    *(temp_buff) = *(mybuffer);
    *(temp_buff+1) = *(mybuffer+1);
    *(temp_buff+2) = *(mybuffer+2);
}

bool isBuffFull(){
    if(myStrLen(mybuffer) >=3) return true;
    else return false;
}


