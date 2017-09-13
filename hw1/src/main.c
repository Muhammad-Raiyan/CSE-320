#include <stdlib.h>

#include "hw1.h"
#include "debug.h"
#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif
#include "util.h"

int main(int argc, char **argv)
{
    unsigned short mode;

    mode = validargs(argc, argv);
    char input[1000];
    debug("Mode: 0x%X", mode);

    if(mode == 0){
        printf("%s\n", "FAIL");
        USAGE(argv[0], EXIT_FAILURE);
        return EXIT_FAILURE;
    }
    else if(mode & 0x8000) {
        USAGE(argv[0], EXIT_SUCCESS);
        return EXIT_SUCCESS;
    }

    else {
        char c;
        while( (c = getchar()) != EOF){
            if(c != '\n' && c != '\t' && c != ' '){
                if(((mode >> 13) & 1) == 0) {  // CHECK IF BIT 13 IS 1
                    if( encryptP(c)!= true)
                        return EXIT_FAILURE;
                }
                else{
                    char c2 = getchar();
                    if( decryptP(charToInt(c), charToInt(c2))!= true)
                        return EXIT_FAILURE;
                }
            }
            else {
                printf("%c", c);
            }
        }
    }
}

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */