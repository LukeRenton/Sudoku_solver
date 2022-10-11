#include <iostream>
#include <unordered_set>
#include <map>
#pragma optimization_level 3
#pragma GCC optimize("Ofast")
#pragma GCC target("avx2")

using namespace std;

const int grid_size = 9; //board size
unordered_set<char> notes[grid_size][grid_size] = {}; //each element is an unordered set! Thus can store multiple numbers for each cell

void populate_grid(char grid[grid_size][grid_size]){
	for (int i=0;i<grid_size;++i){
		for (int j=0;j<grid_size;++j){
			cin >> grid[i][j];
		}
	}
}

void display_grid(char grid[grid_size][grid_size]){
	for (int i = 0; i < grid_size; ++i){
        cout << grid[i][0];
        for (int j = 1 ; j < grid_size; ++j){
            cout<< ' ' << grid[i][j];
        }
    cout << "\n";
    }
}

bool valid_sudoku(char grid[grid_size][grid_size], char input, int row, int col){
	
    int box_start_row = (row < 3) ? 0 : (row < 6) ? 3 : 6; //sets the row start of box
    int box_start_col = (col < 3) ? 0 : (col < 6) ? 3 : 6; //sets the col start of the box
	char temp_row, temp_col, temp_box;
    for (int i=0;i<grid_size;i++){
    	
    	temp_row = grid[row][i];
    	temp_col = grid[i][col];
    	
    	//change the col every 3 rows
    	if (!(i%3) && i != 0){
			box_start_col += 1;	
		}
		
		temp_box = grid[box_start_row + (i % 3)][box_start_col];
		
		if (temp_row == input || temp_col == input  || temp_box == input){
			return false;
		}
	}
    return true;
}

void remove_notes(char grid[grid_size][grid_size], int row, int col, char input){ //this is working!
	
	int box_start_row = (row < 3) ? 0 : (row < 6) ? 3 : 6; 
    int box_start_col = (col < 3) ? 0 : (col < 6) ? 3 : 6;
    
	for (int i=0;i<9;i++){
		
    	if (grid[row][i] == '0'){
    		notes[row][i].erase(input);
    	}
    	
    	if (grid[i][col] == '0'){
    		notes[i][col].erase(input);

		}
		
		if (!(i%3) && i != 0){
			box_start_col += 1;	
		}
		
		if (grid[box_start_row + (i % 3)][box_start_col] == '0'){
			notes[box_start_row + (i % 3)][box_start_col].erase(input);
		
			
		}
	}
}

bool best_guess(char grid[grid_size][grid_size], int &row, int &col){
	int min = grid_size;
	bool completed_grid = true;
	for (int i=0;i<grid_size;++i){
		for (int j=0;j<grid_size;++j){
			if (grid[i][j] == '0'){
				if (notes[i][j].size()<min){
					row = i;
					col = j;
					min = notes[i][j].size();
					completed_grid = false;
				}
			}
		}
	}
	return completed_grid;
}


void build_notes(char grid[grid_size][grid_size]){ 
	for (int i=0;i<grid_size;i++){
		for (int j=0;j<grid_size;j++){
			if (grid[i][j] == '0'){
				for (int guess=0;guess<grid_size; guess++){
					if (valid_sudoku(grid, guess+'1', i, j)){
						notes[i][j].insert(guess+'1');
					}
				}
			}
		}
	}
}

//hidden single - check if in the row/box/col theres only one hint in one box an nowhere else
bool hidden_single(char grid[grid_size][grid_size]){ //check box, check row and check col
	bool changed = false;
	
	//Hidden single for row
	for (int i=0;i<grid_size;i++){ 
		map<char, int> found;
		for (int j=0;j<grid_size;j++){
			if (grid[i][j] == '0'){
				for (char num : notes[i][j]){
					found[num]++;
				}
			}
		}	

		for (auto num : found){
			if (num.second == 1){
				for (int col=0;col<grid_size;col++){
					if (grid[i][col] == '0'){
						if (notes[i][col].find(num.first) != notes[i][col].end()){
							grid[i][col] = num.first;
							remove_notes(grid, i, col, num.first);
							changed = true;
						}
					}
				}
			}
		}
		
	}
	
	//hidden single for all columns
	for (int i=0;i<grid_size;i++){ 
		map<char, int> found;
		for (int j=0;j<grid_size;j++){
			if (grid[j][i] == '0'){
				for (char num : notes[j][i]){
					found[num]++;
				}
			}
		}	

		for (auto num : found){
			if (num.second == 1){
				for (int row=0;row<grid_size;row++){
					if (grid[row][i] == '0'){
						if (notes[row][i].find(num.first) != notes[row][i].end()){
							grid[row][i] = num.first;
							remove_notes(grid, row, i, num.first);
							changed = true;
						}
					}
				}
			}
		}
	}
    return changed;
}

bool naked_single(char grid[grid_size][grid_size]){ //this is working!
	char input;
	bool changed = false;
	for (int i=0;i<grid_size;i++){
		for (int j=0;j<grid_size;j++){
			if (grid[i][j] == '0'){
				if (notes[i][j].size() == 1){ //theres only one option here
					changed = true;
					input = *notes[i][j].begin();
					grid[i][j] = input; //make the guess
					remove_notes(grid, i, j, input);
				}
			}
		}
	}
	return changed;
}

void natural_deduction(char grid[grid_size][grid_size]){
	while (true){
		if (naked_single(grid)){
			continue;
		}
		
		
		if (hidden_single(grid)){
			continue;
		}
		
		break;
	}
}



bool solve_grid(char grid[grid_size][grid_size]){
	int row = 0, col = 0;

	if (best_guess(grid, row, col)){ 
		return true;
	}
	
	for (int i=0;i<grid_size;i++){
		if (valid_sudoku(grid, i+'1', row, col)){
			grid[row][col] = i+'1';
			if (solve_grid(grid)){
				return true;
			}
			grid[row][col] = '0';
		}
	}
	return false;
}

int main(){
	char grid[grid_size][grid_size];
	
	
	//Optimise I/O
	ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    
	populate_grid(grid);
	
	build_notes(grid);
	
	natural_deduction(grid);
	
	if (solve_grid(grid)){
		display_grid(grid);
	}
	else{
		cout << "No Solution" << endl;
	}
	return 0;
	
}
