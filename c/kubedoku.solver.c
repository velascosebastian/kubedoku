///////////////////////////////////////////////////////////////////////////////
// Kubedoku a Sudoku Solver
//
// by Sebastian Isaac Velasco - velascosebastian@gmail.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2013 Sebastian Isaac Velasco
//
///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
// Sudoku Definitions
///////////////////////////////////////////////////////////////////////////////
#define SIZE 	9 	// 9x9 Sudoku
#define SIZE_ 	10 	// 9x9 Sudoku
#define DIM 	3 	// 9x9 Sudoku
#define DIM_ 	4 	// 9x9 Sudoku

///////////////////////////////////////////////////////////////////////////////
// Sudoku Data Structures
///////////////////////////////////////////////////////////////////////////////
typedef struct _game {
	int result[SIZE_][SIZE_];
	int cube[SIZE_][SIZE_][SIZE_];
	int square[DIM_][DIM_][SIZE_];
	int level;
} Game;

typedef struct _option {
	int x;
	int y;
	int z;
} Option;

typedef struct _options {
	int n;
	Option* value;
} Options;

///////////////////////////////////////////////////////////////////////////////
// Sudoku Function Prototypes
///////////////////////////////////////////////////////////////////////////////
void kubedoku(char *sudoku);
void init_game(Game *game, const char *values);
int solve_sudoku(Game *game);
void set_value(int x, int y, int z, Game *game);
inline void set_result(int x, int y, int z, Game *game);
inline void set_counts(int x, int y, int z, Game *game);
void get_value(Game *game);
void get_value_square(Game *game);
Options *get_options(Game *game, Options *options);
void get_result(Game *game, char *string);

///////////////////////////////////////////////////////////////////////////////
// Sudoku Functions
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////
// Main Function to resolve Sudoku Game
/////////////////////////////////////////////////
void kubedoku(char *sudoku) {
	Game *game = (Game*)malloc(sizeof(Game));
	init_game(game, sudoku);
	solve_sudoku(game);
	get_result(game, sudoku);
}

/////////////////////////////////////////////////
// Create all the Game Data Structures
/////////////////////////////////////////////////
void init_game(Game *game, const char *values) {
	int x, y, z;
	int valx, valy, valz;
	for (x = 0; x < SIZE_; x++) {
		valx = (x ? 1 : SIZE);
		for (y = 0; y < SIZE_; y++) {
			game->result[x][y] = 0;
			valy = (y ? 1 : SIZE);
			for (z = 0; z < SIZE_; z++) {
				valz = (z ? 1 : SIZE);
				game->cube[x][y][z] = valx * valy * valz;
			}
		}
	}
	for (x = 0; x < DIM_; x++) {
		for (y = 0; y < DIM_; y++) {
			for (z = 0; z < DIM_; z++) {
				game->square[x][y][z] = SIZE;
			}
		}
	}
	game->level = 0;
	// Set the Initial Values
	int n;
	for (n = 0; n < (SIZE * SIZE); n++) {
		x = (n % SIZE) + 1;
		y = ((int) (n / SIZE)) + 1;
		char c = values[n];
		z = atoi(&c);
		if (z) {
			set_value(x, y, z, game);
		}
	}
}

/////////////////////////////////////////////////
// Recursive Function to Solve the Sudoku
/////////////////////////////////////////////////
int solve_sudoku(Game *game) {
	int cube000, n;
	while (1) {
		cube000 = game->cube[0][0][0];
		get_value(game);
		get_value_square(game);
		if (game->result[0][0] == 81) {
			return 1; // Game OK !!!
		}
		if (cube000 == game->cube[0][0][0]) {
			if (game->cube[0][0][0] == 0) {
				return 0; // Game Wrong !!!
			}
			Options options[SIZE_];
			for (n = 2; n < SIZE_; n++) {
				options[n].n = 0;
				options[n].value = (Option*)malloc(n * sizeof(Option));
			}
			Options *values = get_options(game, options);
			if ((*values).n) {
				for (n = 0; n < (*values).n; n++) {
					Game *game2 = (Game*)malloc(sizeof(Game));
					memcpy(game2, game, sizeof(Game));
					game2->level  = game->level + 1;
					set_value((*values).value[n].x, (*values).value[n].y, (*values).value[n].z, game2);
					int result = solve_sudoku(game2);
					if (result == 1) {
						memcpy(game, game2, sizeof(Game));
						return 1;
					}
				}
			}
			return 0;
		}
	}
}

/////////////////////////////////////////////////
// Set value and clean axis and square
/////////////////////////////////////////////////
void set_value(int x, int y, int z, Game *game) {
	set_result(x, y, z, game);
	int i, j;
	for (i = 1; i < SIZE_ ; i++) {
		// Clean "X" axis
		if (game->cube[i][y][z] == 1) {
			game->cube[i][y][z] = 0;
			set_counts(i, y, z, game);
			}
		// Clean "Y" axis
		if (game->cube[x][i][z] == 1) {
			game->cube[x][i][z] = 0;
			set_counts(x, i, z, game);
			}
		// Clean "Z" axis
		if (game->cube[x][y][i] == 1) {
			game->cube[x][y][i] = 0;
			set_counts(x, y, i, game);
			}
		}
	// Clean Squares
	int xs = ((int) ((x - 1) / DIM)) * DIM;
	int ys = ((int) ((y - 1) / DIM)) * DIM;
	for (i = xs+1; i < (xs+DIM_); i++) {
		for (j = ys+1; j < (ys+DIM_); j++) {
			if (game->cube[i][j][z] == 1) {
				game->cube[i][j][z] = 0;
				set_counts(i, j, z, game);
			}
		}
	}
}

/////////////////////////////////////////////////
// Update the Results Matrix
/////////////////////////////////////////////////
inline void set_result(int x, int y, int z, Game *game) {
	game->result[x][y] = z;
	game->result[x][0]++;
	game->result[0][y]++;
	game->result[0][0]++;
}

/////////////////////////////////////////////////
// Update the Counters in the Matrix & Square
/////////////////////////////////////////////////
inline void set_counts(int x, int y, int z, Game *game) {
	// Planes
	game->cube[0][y][z]--;
	game->cube[x][0][z]--;
	game->cube[x][y][0]--;
	game->cube[0][0][0]--;
	// Squares
	x = ((int) (x - 1) / DIM) + 1;
	y = ((int) (y - 1) / DIM) + 1;
	game->square[x][y][z]--;
}

/////////////////////////////////////////////////
// Get Values from the Cube Matrix
/////////////////////////////////////////////////
void get_value(Game *game) {
	int i, j, k;
	for (i = 1; i < SIZE_; i++) {
		for (j = 1; j < SIZE_; j++) {
			// Plane xy
			if (game->cube[i][j][0] == 1) {
				for (k = 1; k < SIZE_; k++) {
					if (game->cube[i][j][k] == 1) {
						set_value(i, j, k, game);
					}
				}
			}
			// Plane xz
			if (game->cube[i][0][j] == 1) {
				for (k = 1; k < SIZE_; k++) {
					if (game->cube[i][k][j] == 1) {
						set_value(i, k, j, game);
					}
				}
			}
			// Plane yz
			if (game->cube[0][i][j] == 1) {
				for (k = 1; k < SIZE_; k++) {
					if (game->cube[k][i][j] == 1) {
						set_value(k, i, j, game);
					}
				}
			}
		}
	}
}

/////////////////////////////////////////////////
// Get Values from the Squares
/////////////////////////////////////////////////
void get_value_square(Game *game) {
	int x, y, z, i, j;
	for (x = 1; x < DIM_; x++) {
		for (y = 1; y < DIM_; y++) {
			Z: for (z = 1; z < SIZE_; z++) {
				if (game->square[x][y][z] == 1) {
					for (i = ((x - 1) * DIM + 1); i < ((x * DIM) + 1) ; i++) {
						for (j = ((y - 1) * DIM + 1); j < ((y * DIM) + 1) ; j++) {
							if (game->cube[i][j][z] == 1) {
								set_value(i, j, z, game);
								goto Z;
							}
						}
					}
				}
			}
		}
	}
}

/////////////////////////////////////////////////
// Get Optional Values from the Line Counters
/////////////////////////////////////////////////
Options *get_options(Game *game, Options *options) {
	int i, j, k;
	for (i = 1; i < SIZE_; i++) {
		for (j = 1; j < SIZE_; j++) {
			// Plane xy
			if (game->cube[i][j][0] > 1) {
				int n1 = game->cube[i][j][0];
				if (options[n1].n == 0) {
					int n2 = 0;
					for (k = 1; k < SIZE_; k++) {
						if (game->cube[i][j][k] == 1) {
							options[n1].value[n2].x = i;
							options[n1].value[n2].y = j;
							options[n1].value[n2].z = k;
							n2++;
						}
					}
					options[n1].n = n2;
					if (n1 == 2) {
						return &options[2];
					}
				}
			}
			// Plane xz
			if (game->cube[i][0][j] > 1) {
				int n1 = game->cube[i][0][j];
				if (options[n1].n == 0) {
					int n2 = 0;
					for (k = 1; k < SIZE_; k++) {
						if (game->cube[i][k][j] == 1) {
							options[n1].value[n2].x = i;
							options[n1].value[n2].y = k;
							options[n1].value[n2].z = j;
							n2++;
						}
					}
					options[n1].n = n2;
					if (n1 == 2) {
						return &options[2];
					}
				}
			}
			// Plane yz
			if (game->cube[0][i][j] > 1) {
				int n1 = game->cube[0][i][j];
				if (options[n1].n == 0) {
					int n2 = 0;
					for (k = 1; k < SIZE_; k++) {
						if (game->cube[k][i][j] == 1) {
							options[n1].value[n2].x = k;
							options[n1].value[n2].y = i;
							options[n1].value[n2].z = j;
							n2++;
						}
					}
					options[n1].n = n2;
					if (n1 == 2) {
						return &options[2];
					}
				}
			}
		}
	}
	for (i = 3; i < SIZE_; i++) {
		if (options[i].n) {
			return &options[i];
		}
	}
	return &options[2];
}

/////////////////////////////////////////////////
// Get the Sudoku Values
/////////////////////////////////////////////////
void get_result(Game *game, char *string) {
	int n = 0;
	int x, y;
	for (y = 1; y < SIZE_; y++) {
		for (x = 1; x < SIZE_; x++) {
			string[n++] = (game->result[x][y] ? game->result[x][y] + 48 : '.');
			}
		}
	string[n] = '\0';
	}
