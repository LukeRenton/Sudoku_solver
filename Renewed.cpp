#include <cstdio>
#include <chrono>
#include <bitset>
#pragma GCC optimize("Ofast")
#pragma GCC target("avx,avx2,fma")


using namespace std;

const int grid_size = 9;
bitset<grid_size> row_contains[grid_size] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
bitset<grid_size> col_contains[grid_size] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
bitset<grid_size> box_contains[grid_size] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
bitset<grid_size> notes[grid_size][grid_size] = {};

size_t get_cell(size_t row, size_t col){
	return (row / 3) * 3 + col / 3;
}

size_t get_next_row(size_t row, size_t col){
	return row + (col + 1) / 9;
}

size_t get_next_col(size_t col){
	return (col + 1) % 9;
}

void populate_grid(char grid[grid_size][grid_size]){
	char num;
	for (int i=0;i<grid_size;++i){
		for (int j=0;j<grid_size;++j){
			scanf(" %c", &num);
			grid[i][j] = num;
			if (num != '0'){
				size_t index = num - '1';
				row_contains[i].set(index);
				col_contains[j].set(index);
				box_contains[get_cell(i, j)].set(index);
			}
		}
	}
}

void display_grid(char grid[grid_size][grid_size]){
    for (int i = 0; i < 9; ++i){
        printf("%c ", grid[i][0]);
        for (int j = 1 ; j < 8; ++j){
        	printf("%c ", grid[i][j]);
        }
        printf("%c", grid[i][grid_size-1]);
    printf("\n");
    }
}

int next_empty_position(char grid[grid_size][grid_size], size_t &row, size_t &col){
	
	while (row != 9){
        if (grid[row][col] == '0'){
            return row, col;
        }
        row = get_next_row(row, col);
        col = get_next_col(col);
    }

        return 9, 0;	

}

bool solve_grid(char grid[grid_size][grid_size], size_t row_start, size_t col_start){
	
	int row, col = next_empty_position(grid, row_start, col_start);
	if (row_start == 9){
		return true;
	}
	
	size_t const cell = get_cell(row_start, col_start);
	
	bitset<grid_size> contains = row_contains[row_start] | col_contains[col_start] | box_contains[cell];
	
	if (contains.all()){//pretty sure this is useless?
		return false;
	}
	
	for (size_t i=0;i<grid_size;i++){
		if (!contains[i]){
			grid[row_start][col_start] = i+'1';
			row_contains[row_start].set(i);
        	col_contains[col_start].set(i);
        	box_contains[cell].set(i);
        	
        	if (solve_grid(grid, row_start, col_start)){
                return true;
            }
        	row_contains[row_start].reset(i);
        	col_contains[col_start].reset(i);
        	box_contains[cell].reset(i);
		}
	}
	grid[row_start][col_start] = '0';
	return false;
}


int main(){
	char grid[grid_size][grid_size];
	
	populate_grid(grid);
	
	if (solve_grid(grid, 0, 0)){

		display_grid(grid);
		
	}
	else{
		printf("No Solution\n");
	}
	
	 
	
	return 0;
}
