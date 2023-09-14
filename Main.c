#include <stdio.h>
#include <limits.h> // for INT_MAX and INT_MIN
#include <string.h>

// don't tinker with these
#define player 'X'
#define AI '0'
#define m 6
#define n 7
#define max(a, b) ((a >= b) ? a : b)
#define min(a, b) ((a <= b) ? a : b)

// you can modify this: depth at which heuristic is computed. Lower is faster
#define depth 8

// global variables
int baseMove = 0;
char board[m][n];
int lastOfficialMove[2]; // coordinates of the last move made on the real board

// for the heuristic function evaluateContent()
int evaluationTable [6][7] = {{3, 4, 5, 7, 5, 4, 3},
            {4, 6, 8, 10, 8, 6, 4},
            {5, 8, 11, 13, 11, 8, 5},
            {5, 8, 11, 13, 11, 8, 5},
            {4, 6, 8, 10, 8, 6, 4},
            {3, 4, 5, 7, 5, 4, 3}};

int evaluateContent() { 
  // returns the heuristic value of a position based on the evaluation table above
  // heuristic function source: https://softwareengineering.stackexchange.com/questions/263514/why-does-this-evaluation-function-work-in-a-connect-four-game-in-java
        int sum = 0;
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                if (board[i][j] == player)
                    sum += evaluationTable[i][j];
                else if (board[i][j] == AI)
                    sum -= evaluationTable[i][j];
        return sum;
  }

void makeBoard(){ // initializes empty board
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            board[i][j] = ' ';
        }
    }
}

void displayBoard(){
    for (int i = 0; i < m; i++){
        if (i == 0) {
            printf("\n-----------------------------\n");
        }
        for(int j = 0; j < n; j++){
            printf("| %c ", board[i][j]);
            if (j == n - 1){
                printf("|");
            }
        }
        printf("\n-----------------------------\n");
    }
    printf("  0   1   2   3   4   5   6\n");
}

int isValid(int move){
  // returns 1 if a move is valid, else 0
    if(move < 0 || move >= n || board[0][move] != ' '){
        return 0;
    }
  return 1;
}


// places a move on the board and updates the lastMove memory location specified
void makeMove(int move, char playerSymbol, int lastMove[2]){
  for(int i = m - 1; i >= 0; i--){
        if (board[i][move] == ' '){ // lowest unfilled entry in the column
            board[i][move] = playerSymbol;
            lastMove[0]= i;
            lastMove[1] = move;
            return;
        }
    }
}


int checkGame(int totalMoves, int lastMove[2]) { 
  // checks if the game is over
  // returns 1 if player won,
  // returns 2 if the board is filled
  // returns 0 if the game is not over
  // returns -1 if the AI won
  
    if (totalMoves > m * n) { // max amounts of moves in the game for a mxn connect 4 board
        return 2;
    }

    int playerChecker = 0; // variable used to check if there are 4 in a row
    int AIChecker = 0;
  
    // Check vertical
    for (int i = 0; i < m; i++) {
        if (board[i][lastMove[1]] == player) {
            playerChecker++;
            if (playerChecker == 4) {
                return 1;
            }
          AIChecker = 0;
        } else if (board[i][lastMove[1]] == AI) {
            AIChecker ++;
            if (AIChecker == 4){
              return -1;
            }
          playerChecker = 0;
        }
        else{
          playerChecker = 0;
          AIChecker = 0;
        }
    }

    // Check horizontal
    playerChecker = AIChecker = 0; // reset values
    for (int j = 0; j < n; j++) {
        if (board[lastMove[0]][j] == player) {
            playerChecker++;
            if (playerChecker == 4) {
                return 1;
            }
          AIChecker = 0;
        } else if (board[lastMove[0]][j] == AI) {
            AIChecker ++;
            if (AIChecker == 4){
              return -1;
            }
          playerChecker = 0;
        }
        else{
          playerChecker = 0;
          AIChecker = 0;
        }
    }

    // Check diagonal (negative slope)
    playerChecker = AIChecker = 0;
    int i = lastMove[0] - min(lastMove[0], lastMove[1]);
    int j = lastMove[1] - min(lastMove[0], lastMove[1]);
    while (i < m && j < n) {
        if (board[i][j] == player) {
            playerChecker++;
            if (playerChecker == 4) {
                return 1;
            }
          AIChecker = 0;
        } else if (board[i][j] == AI) {
            AIChecker ++;
            if (AIChecker == 4){
              return -1;
            }
          playerChecker = 0;
        }
        else{
          playerChecker = 0;
          AIChecker = 0;
        }
        i++;
        j++;
    }

    // Check diagonal (positive slope)
    playerChecker = AIChecker = 0;
    i = lastMove[0] + min(m - 1 - lastMove[0], lastMove[1]);
    j = lastMove[1] - min(m - 1 - lastMove[0], lastMove[1]);
    while (i >= 0 && j < n) {
        if (board[i][j] == player) {
            playerChecker++;
            if (playerChecker == 4) {
                return 1;
            }
          AIChecker = 0;
        } else if (board[i][j] == AI) {
            AIChecker ++;
            if (AIChecker == 4){
              return -1;
            }
          playerChecker = 0;
        }
        else{
          playerChecker = 0;
          AIChecker = 0;
        }
        i--;
        j++;
    }
    return 0; // game is incomplete
}

int miniMax(int turns, char currentPlayer, int alpha, int beta, int lastMove[2]){ 
    // miniMax algorithm with alpha-beta pruning
    // positive is player
    // negative is AI
  
    int gameState = checkGame(turns, lastMove);
    if (gameState == 2) {
        return 0;
    }
    else if (gameState == 1) {
        return 1000 - turns;
    }
    else if (gameState == -1) {
        return -1000 + turns;
    }
    else if (turns == baseMove + depth){
      return evaluateContent(); // default to heuristic at given depth
    }

    int lastMoveTemp[2] = {lastMove[0], lastMove[1]}; // recursively stores the next set of moves played, so they can be undone
    turns++;
    int bestScore;

    //maximizing tree 
    if (currentPlayer == player) { 
        bestScore = INT_MIN;
        for (int j = 0; j < n; j++) {
            if (isValid(j)) {
                makeMove(j, player, lastMoveTemp);
                int score = miniMax(turns, AI, alpha, beta, lastMoveTemp);
                board[lastMoveTemp[0]][lastMoveTemp[1]] = ' '; // undo move
                bestScore = max(score, bestScore);
                alpha = max(bestScore, alpha);
                if (alpha >= beta) {
                  break;
                }
            }
        }
    } // end maximizer
        // minimizing tree
    else {
        bestScore = INT_MAX;
        for (int j = 0; j < n; j++) {
            if (isValid(j)) {
                makeMove(j, AI, lastMoveTemp);
                int score = miniMax(turns, player, alpha, beta, lastMoveTemp);
                board[lastMoveTemp[0]][lastMoveTemp[1]] = ' '; // undo move
                bestScore = min(bestScore, score);
                beta = min(bestScore, beta);
              
                if (alpha >= beta) {
                 break;
                }
            }
        }
    } // end minimizer
  return bestScore;
}

// simulates a computer move
// used to access Minimax
void compMove(int turns){ 
    int bestScore = INT_MAX;
    int bestMove = -1;
    int alpha = INT_MIN;
    int beta = INT_MAX;
    baseMove = turns; // sets floor off of which depth is determined

    for (int j = 0; j < n; j++){
        int lastMoveTemp[2];
        if(isValid(j)){
            makeMove(j, AI, lastMoveTemp);
            int score = miniMax(turns + 1, player, alpha, beta, lastMoveTemp);
            board[lastMoveTemp[0]][lastMoveTemp[1]] = ' '; // undo move
            if (score < bestScore){
                bestMove = j;
                bestScore = score;
            }
        }
    }
    makeMove(bestMove, AI, lastOfficialMove);
}


int main() {
    int currentPlayer; // moves first or second
    int totalMoves = 0;

    makeBoard(); // initialize board

    printf("Would you like to play (1)st or (2)nd\n");
    scanf("%d", &currentPlayer);

    // gameloop
    while(1) {
        if (currentPlayer == 1) {
            int playerMove; // move scanned from input
            displayBoard();
            printf("It's your move! Please pick a column (0-6) to drop your piece.\n");
            scanf("%d", &playerMove);


            if (!isValid(playerMove)) {
                printf("That move is invalid! Please try again.\n");
                while (getchar() != '\n') {}
                continue;
            }
            makeMove(playerMove, player, lastOfficialMove);
        }

        else {
            compMove(totalMoves);
        }


        switch (checkGame(totalMoves, lastOfficialMove)) {
            case 1:
                displayBoard();
                printf("The game is over! Player won!");
                return 0;
            case -1:
                displayBoard();
                printf("The game is over! AI won!");
                return 0;
            case 2:
                displayBoard();
                printf("It's a tie!");
                return 0;
        }

        totalMoves++;
        currentPlayer = (currentPlayer == 1) ? 2 : 1;
    } // end gameloop
} // end main
