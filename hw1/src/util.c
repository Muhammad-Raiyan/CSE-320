#include "util.h"

int myToInt(char *str){

    int result = 0;

    for(int i=0; *(str+i)!= '\0'; i++){
        result = result * 10 + *(str+i) - '0';
    }

    return result;
}

void printBits(unsigned short mode){
    while(mode){
        if( mode & 1) printf("1 ");
        else printf("0 ");

        mode >>= 1;
    }
    printf("\n");
}

int myStrLen(char *str){

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