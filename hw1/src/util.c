#include "util.h"

int strToInt(char *str){

    int result = 0;

    for(int i=0; *(str+i)!= '\0'; i++){
        result = result * 10 + *(str+i) - '0';
    }

    return result;
}

int charToInt(char cand){
    if(cand >= '0' && cand <= '9') return cand - '0';
    else if(cand >= 'A' && cand <='F') return cand-'A'+10;
    else return cand-'a'+10;
}

void printBits(unsigned short mode){
    while(mode){
        if( mode & 1) printf("1 ");
        else printf("0 ");

        mode >>= 1;
    }
    printf("\n");
}

int myStrLen(const char *str){

    int len = 0;
    for (int i = 0; *(str + i) != '\0'; ++i)
    {
        len++;
    }
    return len;
}

void display(char *k,int r,int c){
    int i,j;
    char *z;

    for(i=0;i<r;i++){
        z=k+i*c;
        printf("Row %d: ", i);
        for(j=0;j<c;j++){
            char *cand =(z+j);
            printf("%c ", *cand);
            //if(*cand == '\0') printf("NULL ");
        }
        printf("\n");
    }
}

char* myStrContains(char *str, char c){
    while (*str != (char) c) {
        if (!*str++) {
            return NULL;
        }
    }
    return (char *)str;
}

void printArr(char *k, int length){
    for(int i = 0; i<length; i++){
        printf("%c ", *(k+i));
    }
}

void clearArray(char *str){
    int len = myStrLen(str);
    for(int i = 0; i < len; i++){
        *(str+i) = '\0';
    }
}

bool myStrCmp ( const char *str1, const char *str2 )
{
   while ( *str1 != '\0' || *str2 != '\0') {
       if ( *str1 != *str2 ) return false;
       ++str1;
       ++str2;
   }
   return true;
}

int indexOf(const char* str, char target){
    for(int i=0; i < myStrLen(str); i++){
        if(*(str+i) == target) return i;
    }
    return -1;
}