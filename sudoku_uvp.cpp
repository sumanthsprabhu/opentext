#include "common.h"
#include <set>
#include <queue>
#include <assert.h>
#include <map>

using namespace std;


bool unit_prop(pair<int, int> curcell, set<int> & possibleVals, const vector<pair<int,int> > & neighbours, vector<vector<int> > & input_grid)
{
  
  int sizebefore = possibleVals.size();
  for (auto neighbour : neighbours) {
    possibleVals.erase(input_grid[neighbour.first][neighbour.second]);
  }
  int sizeafter = possibleVals.size();
  
  assert(sizeafter > 0 && "Something is wrong in unit prop");
  if (sizeafter == 1) {
    input_grid[curcell.first][curcell.second] = *possibleVals.begin();
  }
  if (sizeafter < sizebefore) {
    return true;
  } else {
    return false;
  }  
}

void get_neighbours(int r, int c, vector<pair<int, int> > & neighbours)
{
  int sqrows = sqrt(ROWS);
  int sqcols = sqrt(COLS);
  int minsgr;
  int minsgc;
  int maxsgr;
  int maxsgc;

  //Minimum and maximum row coordinates for subgrid
  for (int i = sqrows; i <= ROWS; i += sqrows) {
    if (r < i) {
      minsgr = i-sqrows;
      maxsgr = i-1;
      break;
    }
  }

  //Minimum and maximum coulmn coordinates for subgrid
  for (int i = sqcols; i <= COLS; i += sqcols) {
    if (c < i) {
      minsgc = i-sqcols;
      maxsgc = i-1;
      break;
    }
  }

  //Same columns  
  for (int i = 0; i < ROWS; i++) {
    if (i != r) {
      neighbours.push_back({i,c});
    }
  }

  //Same row  
  for (int i = 0; i < COLS; i++) {
    if (i != c) {
      neighbours.push_back({r,i});
    }
  }

  //Same subgrid
  for (int i = minsgr; i <= maxsgr; i++) {
    for (int j = minsgc; j <= maxsgc; j++) {
      if (i != r && j != c) {
	neighbours.push_back({i,j});
      }
    }
  }  
}

bool assign(pair<int, int> & curcell, int value, 
	    const vector<pair<int, int> > &neighbours,
	    const map<pair<int,int>, set<int> > &possibleValues,
	    vector<vector<int> > & input_grid)
{
  // first check if it is safe to assign this value
  for (auto n : neighbours) {
    if (input_grid[n.first][n.second] == value) {
      return false;
    }
  }

  input_grid[curcell.first][curcell.second] = value;

  return true;
}


void get_pending_cells(vector<pair<int,int> > & pendingCells, const map <pair<int,int>, set<int> > & possibleValues)
{
  for (auto pvc : possibleValues) {
    if (pvc.second.size() > 1) {
      pendingCells.push_back(pvc.first);
    }
  }
}

bool solved(const vector<vector<int> > & input_grid,
	    map <pair<int,int>, vector<pair<int, int> > > &neighbours)
{
  //No empty cell
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      if (input_grid[i][j] == 0) {
	cout << "Following cell has zero: " << i << j << endl;
	return false;
      }
      
      set<int> neighbours_values;
      for (auto n : neighbours[{i,j}]) {
	neighbours_values.insert(input_grid[n.first][n.second]);	
      }
      if (neighbours_values.find(input_grid[i][j]) != neighbours_values.end()) {
	cout << "Following cell has duplicate neighbour: " << i << j << endl;
	return false;
      }
    }
  }

  return true;
}


bool search_rec(const vector<pair<int, int> > & pendingCells,
		int cindex,
		map <pair<int,int>, vector<pair<int, int> > > & neighbours,
		map <pair<int,int>, set<int> > & possibleValues,
		vector<vector<int> > & input_grid)
{
  if (cindex >= pendingCells.size()) {
    return true;
  }
  
  pair<int,int> cell = pendingCells[cindex];
  for (auto val : possibleValues[cell]) {
    if (assign(cell, val, neighbours[cell], possibleValues, input_grid)) {
      if (search_rec(pendingCells, cindex+1, neighbours, possibleValues, input_grid)) {
	return true;
      }
    }	  
    input_grid[cell.first][cell.second] = 0;
  }
  return false;
}

  


int solve_sudoku_uvp(vector<vector<int> > input_grid)
{
  
  //initialization
  map <pair<int,int>, vector<pair<int, int> > > neighbours;
  map <pair<int,int>, set<int> > possibleValues;
  queue<pair<int,int> > worklist;
  
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      vector<pair<int, int> > ns;
      get_neighbours(i,j,ns);
      neighbours.insert({{i,j}, ns});

      set<int> vals;
      if (input_grid[i][j] != 0) {
	vals.insert(input_grid[i][j]);
      } else {
	for (int i = 1; i <= ROWS; i++) {
	  vals.insert(i);
	}
      }
      possibleValues.insert({{i,j}, vals});

      if (input_grid[i][j] == 0) {
	worklist.push({i,j});
      }
    }
  }

  //first try unit value propagation
  while (!worklist.empty()) {
    pair<int, int> curcell = worklist.front();
    worklist.pop();
    bool changed = unit_prop(curcell, possibleValues[curcell], neighbours[curcell], input_grid);
    if (changed) {
      for (auto neighbour : neighbours[curcell]) {
	worklist.push(neighbour);
      }
    }
  }

  //now, search everything recursively
  vector<pair<int,int> > pendingCells;
  get_pending_cells(pendingCells, possibleValues);
  search_rec(pendingCells, 0, neighbours, possibleValues, input_grid);

  // verifies the solution
  assert(solved(input_grid, neighbours) && "not solved!");

  //print the solution
  for (auto i = 0; i < ROWS; i++) {
    for (auto j = 0; j < COLS; j++) {
      cout << input_grid[i][j] << "\t";
    }
    cout << endl;
  }
  
  return 0;
  
}
