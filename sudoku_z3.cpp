#ifdef USE_Z3
#include "common.h"
#include "z3++.h"

using namespace std;
using namespace z3;

int solve_sudoku_z3(const vector<vector<int> > & input_grid)
{
  context c;
  solver s(c);
  vector<vector<expr> > grid;
  
  for (int row = 0; row < ROWS; row++) {
    vector<expr> tmp;
    for (int col = 0; col < COLS; col++) {
      tmp.push_back(c.int_const(string(string("grid") + to_string(row) + to_string(col)).c_str()));
    }
    grid.push_back(tmp);
  }

  /* Each cell can have a value between 1 to ROWS */
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      s.add(1 <= grid[row][col]);
      s.add(grid[row][col] <= ROWS);
    }
  }

  /* In each row, all values from 1 to ROWS should be present */
  for (int val = 1; val <= ROWS; val++) {
    for (int row = 0; row < ROWS; row++) {
      expr disj = c.bool_const(string(string("rowval") + to_string(row) + to_string(val)).c_str());
      disj = c.bool_val(false);
      for (int col = 0; col < COLS; col++) {
  	disj = disj || grid[row][col] == val;
      }
      s.add(disj);
    }
  }

  /* In each column, all values from 1 to COLS should be present */
  for (int val = 1; val <= COLS; val++) {
    for (int col = 0; col < COLS; col++) {
      expr disj = c.bool_const(string(string("colval") + to_string(col) + to_string(val)).c_str());
      disj = c.bool_val(false);
      for (int row = 0; row < ROWS; row++) {
  	disj = disj || grid[row][col] == val;
      }
      s.add(disj);
    }
  }

  /* All values from 1 to ROWS should be present in each subgrid*/
  int sqrtrows = sqrt(ROWS);
  int sqrtcols = sqrt(COLS);
  for (int val = 1; val <= ROWS; val++) {
    for (int i = 1; i <= sqrtrows; i++) {
      for (int j = 1; j <= sqrtcols; j++) {
  	expr disj = c.bool_const(string(string("subgridrowcolval") + to_string(i) + to_string(j) + to_string(val)).c_str());
  	disj = c.bool_val(false);	
  	for (int row = (i-1)*sqrtrows; row < i*sqrtrows; row++) {
  	  for (int col = (j-1)*sqrtcols; col < j*sqrtcols; col++) {
  	    disj = disj || grid[row][col] == val;
  	  }
  	}
  	s.add(disj);
      }
    }
  }

  /* constrain already given values */
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      if (input_grid[row][col] != DUMMY_VALUE) {
	s.add(grid[row][col] == input_grid[row][col]);
      }
    }
  }

  /* check if the solver returns a solution that satisfies the constraints*/
  bool solved = false;
  switch (s.check()) {
  case unsat : cout << "No solution!\n"; break;
  case sat : solved = true; break;
  case unknown: cout << "Z3 can't find a solution :( \n"; break;
  }

  if (!solved) {
    return -1 ;
  }

  /* printing solution*/
  model m = s.get_model();
  /* constrain already given values */
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      if (input_grid[row][col] != DUMMY_VALUE) {
  	cout << input_grid[row][col];
      } else {
  	cout << m.eval(grid[row][col]);
      }
      cout << "\t";
    }
    cout << endl;
  }
  
  return 0;
}
#endif
