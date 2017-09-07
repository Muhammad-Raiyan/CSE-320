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

/**
* The mode is 0 if there is any form of failure. This includes, but is not limited to:
*
* Invalid number of arguments (too few or too many)
* Invalid ordering of arguments
* Incorrect arguments for the specified cipher (e.g. -r or -c being passed in with -f)
* Invalid key (if one is specified). A key is invalid if it contains characters not in the alphabet or has repeated characters. Note: The ciphers use different alphabets.
* The number of rows or columns is invalid (i.e. more than 16 or less than 9)
* (rows * columns) < length of polybius_alphabet for the Polybius cipher
*/
bool isModeZero(int argc, char** argv){

    if (argc > 7 || argc < 2){
        return true;
    }

    int pos_count = 0;

    for (char **pt_a = argv + 1; pt_a != argv+argc; pt_a++) {
        for(char *pt_inr = *pt_a + 1; *pt_inr != '\0' ; pt_inr++) {

            if(*pt_inr == 'h') {
                //printf("Found H\n");
                continue;
            }
            switch(pos_count){
                case 0:
                    printf("%s\n", "CASE 1");
                    if(*pt_inr != 'p' && *pt_inr != 'f') return true;
                    break;
                case 1:
                    if(*pt_inr != 'e' && *pt_inr != 'd') return true;
                    printf("%s\n", "Case 2");
                    break;
            }
            pos_count++;
            printf("%c\n", *pt_inr);
        }
    }
    return false;
}


