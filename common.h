#include <iostream>
#include <vector>
#include <string>
#include <cmath>

// tested when ROWS=COLS=9 
const int ROWS = 9;
const int COLS = ROWS;
const int DUMMY_VALUE = 0;
const int TOTAL_PEERS = 20; //ROWS-1 + COLS-1 + (sqrt(ROWS)*sqrt(COLS)-sqrt(ROWS)-sqrt(COLS)+1)
