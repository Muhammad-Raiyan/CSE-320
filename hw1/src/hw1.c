#include "hw1.h"

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the program
 * and will return a unsigned short (2 bytes) that will contain the
 * information necessary for the proper execution of the program.
 *
 * IF -p is given but no (-r) ROWS or (-c) COLUMNS are specified this function
 * MUST set the lower bits to the default value of 10. If one or the other
 * (rows/columns) is specified then you MUST keep that value rather than assigning the default.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return Refer to homework document for the return value of this function.
 */

unsigned short validargs(int argc, char **argv) {
    unsigned short mode = 0x8000;
    printf("argc value: %d\n", argc);

    if(isModeZero(argc, argv) == true){
        return 0;
    }

    return mode;
}

bool isModeZero(int argc, char** argv){

    if (argc > 7 || argc < 2){
        return true;
    }

    char **iterator = argv + 1, indCount = 1;
    if( *(*(argv++)+1)  == 'h') {
        printf("%s\n", "Increment");
        iterator++;
    }


    for(; iterator != argv + argc; iterator++){
        for(char *charItr = *iterator; *charItr != '\0'; charItr++){
            printf("%c ", *charItr);
            if(*charItr == '-') continue;
            switch(indCount){
                case 1:
                    if(*charItr != 'p' && *charItr != 'f') return true;
                    printf("%s\n", "CASE 1");
                    break;
                case 2:
                    if(*charItr != 'e' && *charItr != 'd') return true;
                    printf("%s\n", "CAse 2");
                    break;
            }
        }
    }

    return false;
}


