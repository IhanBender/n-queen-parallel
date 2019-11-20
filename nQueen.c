#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

int testBoard(size_t decimal, int base, float baseLog, int * board){
    if (decimal == 0)
        return 0;

    int quocient;
    int spaces = (int) floor(log(decimal) / baseLog) + 1;
    if (spaces < base - 1)
        return 0;

    for (size_t i = 0; i < spaces; i++)
        board[i] = 0;

    quocient = decimal;
    for (size_t i = base - spaces; i < base; i++) {
        board[i] = quocient % base;
        quocient = quocient / base;

        for (size_t j = 0; j < i; j++) {
            if (board[i] == board[j])           return 0;
            if (board[i] + i == board[j] + j)   return 0;
            if (board[i] - i == board[j] - j)   return 0;
        }
    }

    return 1;
}


int main(int argc, char const *argv[]) {
    const unsigned long int results[21] = {
        -1, 1, 0, 0, 2, 10, 4, 40, 92, 352, 724, 2680, 14200, 73712, 365596,
        2279184, 14772512, 95815104, 666090624, 4968057848, 39029188884
    };
    int boardSize = atoi(argv[1]);
    int numThreads = atoi(argv[2]);
    int ** solutions, totalSolutions, numSolutions = 0;
    int ** boards;
    float sizeLog = log(boardSize);

    if (boardSize > 20 || boardSize < 1){
        printf("Will not be able to calculate with a board size higher than 20 or lower than 1.");
        exit(1);
    }

    totalSolutions = results[boardSize];
    solutions = (int **) malloc(sizeof(int *) * totalSolutions);
    for (size_t i = 0; i < totalSolutions; i++)
        solutions[i] = (int *)malloc(sizeof(int) * boardSize);

    boards = (int **) malloc(sizeof(int*) * numThreads);
    for (size_t i = 0; i < numThreads; i++)
        boards[i] = (int *) malloc(sizeof(int) * boardSize);

    size_t totalPossibilities = (size_t) pow(boardSize, boardSize);
    # pragma omp parallel for schedule(guided) num_threads(numThreads)
    for (size_t i = 0; i < totalPossibilities; i++) {
        if (testBoard(i, boardSize, sizeLog, boards[omp_get_thread_num()])){
            # pragma omp critical
            {
                for (size_t index = 0; index < boardSize; index++)
                    solutions[numSolutions][index] = boards[omp_get_thread_num()][index];
                ++numSolutions;
            }
        }
    }

    printf("Found %d solutions of %d\n", numSolutions, totalSolutions);
    // for (size_t i = 0; i < numSolutions; i++) {
    //     for (size_t j = 0; j < boardSize; j++) {
    //         printf("%d ", solutions[i][j]);
    //     }
    //     printf("\n");
    // }

    for (size_t i = 0; i < numThreads; i++)
        free(boards[i]);
    free(boards);

    for (size_t i = 0; i < totalSolutions; i++)
        free(solutions[i]);
    free(solutions);
    return 0;
}
