#include <stdio.h>
#include <const.h>
#include "util.h"

void createPolybiusTable(int row, int col);
bool encryptP(char ch);
bool decryptP(int row, int col);
void populateTable(const char* words);
int findRow(char ch);
int findCol(char ch);