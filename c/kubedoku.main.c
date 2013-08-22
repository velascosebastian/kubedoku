///////////////////////////////////////////////////////////////////////////////
// Kubedoku a Sudoku Solver
//
// by Sebastian Isaac Velasco - velascosebastian@gmail.com
//
///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kubedoku.solver.c"

int main(int argc, char *argv[]) {
	if ((argc == 1) || (argc > 2) || (strlen(argv[1]) != 81)) {
		printf("Usage: %s \"123456789...\"\n", argv[0]);
		return 1;
	} else {
		char sudoku[82];
		strcpy(sudoku, argv[1]);
		printf("%s\n", sudoku);
		kubedoku(sudoku);
		printf("%s\n", sudoku);
		return 0;
	}
}
