#include "common.h"

using namespace std;

#ifdef USE_Z3
extern int solve_sudoku_z3(const vector<vector<int> > & input_grid);
#else
extern int solve_sudoku_uvp(vector<vector<int> > input_grid);
#endif


int main()
{

  vector<vector<int> > input_grid;

  // cout << "Enter " << ROWS << " x " << COLS << " sudoku " << endl;
  for (int row = 0; row < ROWS; row++) {
    vector<int> r(COLS);
    for (int col = 0; col < COLS; col++) {
      cin >> r[col];
    }
    input_grid.push_back(r);
  }
  
  #ifdef USE_Z3
  solve_sudoku_z3(input_grid);
  #else
  solve_sudoku_uvp(input_grid);
  #endif
  
  return 0;
}
