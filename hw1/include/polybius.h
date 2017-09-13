#include <stdio.h>
#include <const.h>
#include "util.h"

void createPolybiusTable(int row, int col);
bool encryptP(char ch);
char decryptP(int pos);
void populateTable(const char* words);
int findRow(char ch);
int findCol(char ch);