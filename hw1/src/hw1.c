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

//GLOBAL
bool b_fractionated = false;
bool b_decrypt = false;
int row = 10, col = 10;

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
    key = NULL;
    //printf("argc value: %d\n", argc);
    if(*(*(argv+1)+1) == 'h'){
        return mode;
    }

    if(isModeZero(argc, argv) == true){
        return 0;
    }
    mode = 0;
    if(b_fractionated) mode |= 1 << 14;
    if(b_decrypt) mode |= 1 << 13;
    if(row < 9 || row > 15 || col < 9 || col > 15) return 0;
    if(row * col < myStrLen(polybius_alphabet)) return 0;

    if(!b_fractionated){
        mode |= row << 4;
        mode |= col;
        createPolybiusTable(row, col);
    }
    else {
        //printf("Call makeFractionatedKey\n");
        makeFractionatedKey(key);
    }
    //printBits(mode);
    //printf("%04x\n", mode);
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

    if (argc > 9 || argc < 3){
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
                    //printf("%s\n", "CASE 1");
                    if(*pt_inr != 'p' && *pt_inr != 'f') return true;
                    if(*pt_inr == 'f') b_fractionated = true;
                    break;
                case 1:
                    if(*pt_inr != 'e' && *pt_inr != 'd') return true;
                    if(*pt_inr == 'd') b_decrypt = true;
                    //printf("%s\n", "Case 2");
                    break;
            }
            pos_count++;
            if(pos_count > 1){
                if(*pt_inr == 'r'){
                    row =  strToInt( *(pt_a+1) );
                }

                if(*pt_inr == 'c'){
                    col = strToInt( *(pt_a+1) );
                }

                if(*pt_inr == 'k'){
                    if(isValidKey( *( pt_a+1), b_fractionated ))
                        key = *( pt_a+1);
                    else {
                        return true;
                    }
                }
            }
            //printf("%c\n", *pt_inr);
        }
    }
    return false;
}

/*
* Checks if key is valid -
* *For the Polybius cipher, the key must have a non-repeating subset of the characters in the
*  polybius_alphabet variable defined in const.c.
*
* *For the Fractionated Morse Cipher, the key must have a non-repeating subset of the
*  characters in the fm_alphabet variable defined in const.c.
*/
bool isValidKey(char *cand, bool isFractionated){
    const char* table = isFractionated ? fm_alphabet : polybius_alphabet;
    //printf("%s\n", table);

    for(char* pt_ch = cand; *pt_ch != '\0'; pt_ch++){
        char* pt_checkInside = myStrContains((char *)table, *pt_ch);
        if(pt_checkInside!= NULL) {
            char target = *pt_checkInside;
            if(myStrContains(++pt_checkInside, target)!=NULL){
                return false;
            }

        } else {
            return false;
        }
    }
    return true;
}
