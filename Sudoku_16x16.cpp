#include <iostream>
#include <chrono>
#include <unordered_set>
#include <map>
#include <vector>
#include <array>
#pragma GCC optimize("Ofast")
#pragma GCC target("avx,avx2,fma")

using namespace std;
using namespace std::chrono;

const int grid_size = 16; //board size
char guesses[grid_size] = {'1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G'};
unordered_set<char> notes[grid_size][grid_size] = {};
bool clean_up = false;

void populate_grid(char grid[grid_size][grid_size]){
	for (int i=0;i<grid_size;++i){
		for (int j=0;j<grid_size;++j){
			scanf(" %c", &grid[i][j]);
		}
	}
}

void display_grid(char grid[grid_size][grid_size]){
    for (int i=0;i<grid_size;++i){
        printf("%c ", grid[i][0]);
        for (int j=1;j<grid_size-1; ++j){
        	printf("%c ", grid[i][j]);
        }
        printf("%c\n", grid[i][grid_size-1]);
    }
}

bool valid_sudoku(char grid[grid_size][grid_size], char input, int row, int col){//something is wrong here
	
    int box_start_row = (row < 4) ? 0 : (row < 8) ? 4 : (row < 12) ? 8 : 12; //sets the row start of box
    int box_start_col = (col < 4) ? 0 : (col < 8) ? 4 : (col < 12) ? 8 : 12; //sets the col start of the box
    for (int i=0;i<grid_size;i++){
    	
    	char temp_row = grid[row][i];
    	char temp_col = grid[i][col];
    	
    	//change the col every 4 rows
    	if (!(i%4) && i != 0){
			box_start_col += 1;	
		}
		char temp_box = grid[box_start_row + (i % 4)][box_start_col];
		
		if (temp_row == input || temp_col == input  || temp_box == input){
			return false;
		}
	}
    return true;
}

void remove_notes(char grid[grid_size][grid_size], int row, int col, char input){
	
	int box_start_row = (row < 4) ? 0 : (row < 8) ? 4 : (row < 12) ? 8 : 12;
 	int box_start_col = (col < 4) ? 0 : (col < 8) ? 4 : (col < 12) ? 8 : 12; 
	if (!clean_up){
		for (int i=0;i<grid_size;i++){
		
    		if (grid[row][i] == '0'){
    			notes[row][i].erase(input);
    		}
    	
    		if (grid[i][col] == '0'){
    			notes[i][col].erase(input);

			}	
		
			if (!(i%4) && i != 0){
				box_start_col += 1;	
			}
		
			if (grid[box_start_row + (i % 4)][box_start_col] == '0'){
				notes[box_start_row + (i % 4)][box_start_col].erase(input);
			}
		}
	}
	else{
		for (int i=0;i<grid_size;i++){
    		if (grid[row][i] == '0' && (i != col && i != col + 1)){
    			notes[row][i].erase(input);
    		}		
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
//for naked pairs/triples/etc we can just check if any bitstring is identical to another
//in the same row/col/box and if its true remove those candidates from the other ones not neaded



void build_notes(char grid[grid_size][grid_size]){
	for (int i=0;i<grid_size;i++){
		for (int j=0;j<grid_size;j++){
			if (grid[i][j] == '0'){
				for (int guess=0;guess<grid_size; guess++){
					if (valid_sudoku(grid, guesses[guess], i, j)){
						notes[i][j].insert(guesses[guess]);
					}
				}
			}
		}
	}
	
}

unordered_set<char> unordered_set_difference(unordered_set<char> left, int row, int col_left, int col_right){
	for (int i=0;i<grid_size;i++){
		for (auto element : notes[row][i]){
			if (i != col_left || i != col_right){
				left.erase(element);
			}
		}
	}
	return left;
}

unordered_set<char> unordered_set_intersection(unordered_set<char> left, unordered_set<char> right){
	unordered_set<char> result;
	for (auto element : left){
		if (right.count(element) == 1){
			result.emplace(element);
		}
	}
}

bool hidden_single(char grid[grid_size][grid_size]){ //check box, check row and check col
	bool changed = false;
	for (int i=0;i<grid_size;i++){ 
		map<char, int> found_row;
		map<char, int> found_col;
		for (int j=0;j<grid_size;j++){
			if (grid[i][j] == '0'){
				for (char num : notes[i][j]){
					found_row[num]++;
				}
			}	
			if (grid[j][i] == '0'){
				for (char num : notes[j][i]){
					found_col[num]++;
				}
			}
			
		}	
		//Hidden single for row
		for (auto num : found_row){
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
		
	//hidden single for all columns
		for (auto num : found_col){
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

bool naked_tuples(char grid[grid_size][grid_size]){
	for (int i=0;i<grid_size;i++){
		for (int j=0;j<grid_size - 1;j++){
			if (grid[i][j] == '0'){
				if (notes[i][j] == notes[i][j+1]){
					clean_up = true;
					for (auto x : notes[i][j]){
						remove_notes(grid, i, j, x);
					}
					clean_up = false;
					//break;
 				}
 				
 				if (notes[j][i] == notes[j+1][i]){
 					clean_up = true;
 					for (auto p : notes[j][i]){
 						remove_notes(grid, j, i, p);
					 }
					 clean_up = false;
					 break;
				 }
 				
 				
			}
		}
	}
	return false;
}

//if a set of numbers is only found in 2 col/row we can eliminate everything else
//check the intersection of each unordered set, if they intersect with a size greater than 2 we will
//know that its a hidden tuple
//we then replace the notes with that difference
bool hidden_tuples(char grid[grid_size][grid_size]){
	bool changed = false;
	unordered_set<char> contains;
	for (int i=0;i<grid_size;i++){
		for (int j=0;j<grid_size - 1;j++){
			if (notes[i][j].size()>=2){
				for (int k=j+1;k<grid_size;k++){
					if (notes[i][j].size() > 2 || notes[i][k].size() > 2){
						cout << "we made it before int" << endl;
						unordered_set<char> intersection = unordered_set_intersection(notes[i][j], notes[i][k]);
						cout << "we made it after int" << endl;
					
					if (intersection.size() >= 2){
						cout << "we made it before diff" << endl; //we failed here
						unordered_set<char> difference = unordered_set_difference(intersection, i, j, k);
						cout << "we made it after diff" << endl;
						if (difference.size() == 2){
							notes[i][j] = notes[i][k] = difference;
							changed = true;
							break;
						}
					}
				}
				}
				
			}
		}
	}
	
	
	return changed;
}

bool naked_single(char grid[grid_size][grid_size]){
	char input;
	bool changed = false;
	for (int i=0;i<grid_size;++i){
		for (int j=0;j<grid_size;++j){
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
		
		
		if(naked_tuples(grid)){
			continue;
		}
		
		if(hidden_tuples(grid)){
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
		if (valid_sudoku(grid, guesses[i], row, col)){
			grid[row][col] = guesses[i];
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
	populate_grid(grid);
	
	auto start = high_resolution_clock::now();
	build_notes(grid);
	
	natural_deduction(grid);
	
	
	if (solve_grid(grid)){
		display_grid(grid);
	}
	else{
		printf("No Solution\n");
	}
	
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
  	cout << "\nTime taken by function: "
        << duration.count() << " microseconds" << endl;
        
	return 0;
	
}
