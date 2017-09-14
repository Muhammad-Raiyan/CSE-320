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
        if(myStrLen(mybuffer)!= 0){
            cipher();
        }
        clearArray(mybuffer);
        //printf("\n");
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
    if(textToMorseCode(charIndOnMT)==false) return false;
    while(isBuffFull()){
        //printf("Buff is Full: %s\n", mybuffer);
        cipher();
    }

    return encryptF(getchar());
}

void cipher(){
    long space = 0;
    char* target = (char*) &space;
    //printf("%s\n", mybuffer);
    getTriplet(target);
    //printf("REMOVED: %s\n", target);
    printf("%c", cipherMorseCode(target));
    removeFromFront();
}

bool decryptF(char ch){
    int index = indexOf(fm_key, ch);
    //printf("%d\n", index);
    if(index < 0) {
        return true;
    }

    for(int i = 0; i<myStrLen(*(fractionated_table+index)); i++){
        insertEndOfBuffer(*(*(fractionated_table+index)+i));
    }
    //printf("Org: %s\n", polybius_table);
    if(*(polybius_table) == 'x'){
        //if(*(polybius_table+1) != '\0'){
            shiftArrLeft(polybius_table, 1);
            printf(" ");
        //}
    }
    char* tail = findXinBuf();
    if(tail!=NULL){
        //printf("Tail: %c ", *tail);
        morseToText((char *)polybius_table, tail);
    }
    else{
        return decryptF(getchar());
    }
    return true;
}

bool textToMorseCode(int indexOnMorseTable){

    char *tempStr = (char*)*(morse_table + indexOnMorseTable);
    if(myStrCmp(tempStr, ""))
        return false;
    for(int i = 0; *(tempStr+i)!='\0'; i++){
        insertEndOfBuffer(*(tempStr+i));
    }
    insertEndOfBuffer('x');
    return true;
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

void morseToText(char* head, char* tail){
    long space = 0;
    char *cand = (char*)&space;
    int ins_pos = 0;

    while(head != tail){
        *(cand+ins_pos) = *head;
        ins_pos++;
        head++;
    }
    //printf("CAND: %s incr: %d ", cand, ins_pos);

    for(int i = 0; i<'z'-'!'; i++){
        if(myStrCmp(cand, *(morse_table+i))) printf("%c", *(fm_alphabet+i-'!'+1));
    }
    shiftArrLeft(polybius_table, ins_pos+1);
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

char *findXinBuf(){
    int count = 0;
    for(char* p_look = polybius_table; *(p_look)!='\0' ;p_look++){
        if(*p_look == 'x') {
            //printf("Tail Pos: %d\n", count);
            return p_look;
        }
        count++;
    }
    return NULL;
}


