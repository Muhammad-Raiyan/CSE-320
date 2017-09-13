#include <stdio.h>
#include <const.h>
#include "util.h"

void createPolybiusTable(int row, int col);
int encryptP(char ch);
char decryptP(int pos);
void populateTable(const char* words, int orw, int col);