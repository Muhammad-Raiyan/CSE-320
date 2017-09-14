#include <stdio.h>
#include <stdlib.h>
#include <const.h>
#include "util.h"

void makeFractionatedKey(const char* key);
bool encryptF(char ch);
bool decryptF(char ch);
bool textToMorseCode(int indexOnMorseTalbe);
char cipherMorseCode(const char *code);
void insertEndOfBuffer(char ch);
bool isBuffFull();
void removeFromFront();
void getTriplet(char* temp_buff);