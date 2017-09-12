#include "const.h"

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