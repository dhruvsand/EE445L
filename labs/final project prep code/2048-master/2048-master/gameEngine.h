// gameEngine.h
// Provide functions for 2048 logic and graphics.
// Ryan Norton 

//-------------------Board_Init-------------------
// Initializes main playing board. 
// The board is a global variable within gameEngine.c
// Board is indexed by Tile number.
//  -------------------
// | 1  | 2  | 3  | 4  |
// |-------------------|
// | 5  | 6  | 7  | 8  |
// |-------------------|
// | 9  | 10 | 11 | 12 |
// |-------------------|
// | 13 | 14 | 15 | 16 |
//  -------------------
void Board_Init(void);

//--------------Game Logic Functions-------------
int boardTotal(void);
void putValue(int value, int tile);
int getValue(int tile);
int countEmptyTiles(void);
void generateRandomTile(void);
void clearBoard(void);
int checkGameOver(void); // 0 = not gameover, 1 = gameover

//----------------Shift Functions----------------
// Shifts all tiles in one direction such that there
// are no empty tiles between number tiles.
// Manipulates global board.
void shiftLeft(void);
void shiftRight(void);
void shiftUp(void);
void shiftDown(void);

//----------------Merge Functions----------------
// Combines adjacent tiles of same value and returns 
// total score of merged tiles.
// Manipulates global board.
int mergeLeft(void);
int mergeRight(void);
int mergeUp(void);
int mergeDown(void);

//---------------Graphics Functions--------------
// Draws to LCD.
void drawTile(int value, int tileNum);
void drawAllTiles(void);
void writeScore(int score);
void writeHighscore(int score);
void writeTime(int time);
void eraseBoard(void);
void eraseScore(void);
void eraseTime(void);
void displayHighestTile(void);
void drawPauseMode(void);
void drawArrow(void);
void drawGameOver(int score, int time);
void gameOverHighscore(int score);
