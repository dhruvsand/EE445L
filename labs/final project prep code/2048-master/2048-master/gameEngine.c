// Lab10 2048 Game
// gameEngine.c
// 2048 game engine
// Ryan Norton
// Andrew Grado

#include "SSD2119.h"
#include "GameArt.txt"
#include "random.h"
#include "Input.h"
#include "ArrowArt.txt"

short boardX = 113;
short boardY = 28;
short boardLen = 185;
short tilePadding = 5;
unsigned short tileCoords[17][2];
int board[4][4];
int positionDifference[17];

void Board_Init(void){int tileLen, row, col, tileX, tileY;
	
	// Background Color
	LCD_DrawFilledRect(0,0,320,240,BLACK);
	
	// Sidebar Color
	LCD_DrawFilledRect(0,0,91,240,RED);
	LCD_DrawRect(0,0,91,239,BURNTORANGE);	
	
	// Board Frame
	LCD_DrawFilledRect(boardX, boardY, boardLen, boardLen, ORANGE);
	
	// Title
	LCD_DrawBMP(Title, 13, 7);
	
	// Scoreboard/Credits
	LCD_DrawFilledRect(6,59,79,175,BLACK);
	LCD_DrawRect(6,59,79,174,YELLOW);	
	LCD_SetTextColor(255,245,200);	
	LCD_Goto(2,9);	
	printf("Score:");
	LCD_Goto(2,15);
	printf("Highscore:");
	LCD_Goto(2,12);
	printf("Time:");

	// Credits
	LCD_Goto(4,7);
	LCD_SetTextColor(255,255,240);
	printf("LAB 10");	
	LCD_Goto(2,23);
	printf("Created by:");
	LCD_Goto(2,24);
	printf("Ryan Norton");
	LCD_Goto(2,25);
	printf("Andrew Grado");
	
	// Initialize Tile Position Array and Empty Board
	tileLen = (boardLen - (5 * tilePadding)) / 4;
	for (row=0; row<4; row++){
		for (col=0; col<4; col++){
			
			// Calculate tile coordinates
			tileX = row * (tileLen + tilePadding) + tilePadding + boardX;
			tileY = col * (tileLen + tilePadding) + tilePadding + boardY;
			
			// Initialize board with empty tiles
//			LCD_DrawFilledRect(tileX, tileY, tileLen, tileLen, GREEN);
			
			// Initialize tile position array
			tileCoords[col * 4 + row + 1][0] = tileX;
			tileCoords[col * 4 + row + 1][1] = tileY - 1;
		}
	}
	
}

void drawTile(int value, int tileNum) {
	if (value == 2){
		LCD_DrawBMP(Number2, tileCoords[tileNum][0], tileCoords[tileNum][1]);
	}
	else if (value == 4){
		LCD_DrawBMP(Number4, tileCoords[tileNum][0], tileCoords[tileNum][1]);
	}
	else if (value == 8){
		LCD_DrawBMP(Number8, tileCoords[tileNum][0], tileCoords[tileNum][1]);
	}
	else if (value == 16){
		LCD_DrawBMP(Number16, tileCoords[tileNum][0], tileCoords[tileNum][1]);
	}
	else if (value == 32){
		LCD_DrawBMP(Number32, tileCoords[tileNum][0], tileCoords[tileNum][1]);
	}
	else if (value == 64){
		LCD_DrawBMP(Number64, tileCoords[tileNum][0], tileCoords[tileNum][1]);
	}
	else if (value == 128){
		LCD_DrawBMP(Number128, tileCoords[tileNum][0], tileCoords[tileNum][1]);
	}
	else if (value == 256){
		LCD_DrawBMP(Number256, tileCoords[tileNum][0], tileCoords[tileNum][1]);
	}
	else if (value == 512){
		LCD_DrawBMP(Number512, tileCoords[tileNum][0], tileCoords[tileNum][1]);
	}
	else if (value == 1024){
		LCD_DrawBMP(Number1024, tileCoords[tileNum][0], tileCoords[tileNum][1]);
	}
	else if (value == 2048){
		LCD_DrawBMP(Number2048, tileCoords[tileNum][0], tileCoords[tileNum][1]);
	}
}

void drawArrow(void){int x, y;
	// cover old arrow
	LCD_DrawFilledRect(prevX,prevY,20,20,BLACK);

	// draw new arrow
	if (ADCMail < 1024){
		x = 92;
		y = 201;
		y -= (185 * (ADCMail/1024.0));
		LCD_DrawBMP(RightArrow, x, y);
	}
	else if (ADCMail < 2048){
		x = 103;
		x += (185 * ((ADCMail-1024)/1024.0));
		y = 5;
		LCD_DrawBMP(DownArrow, x, y);
	}
	else if (ADCMail < 3072){
		x = 300;
		y = 16;
		y += (185 * ((ADCMail-2048)/1024.0));
		LCD_DrawBMP(LeftArrow, x, y);
	}
	else{
		x = 289;
		x -= (185 * ((ADCMail-3072)/1024.0));
		y = 213;
		LCD_DrawBMP(UpArrow, x, y);
	}	
	prevX = x;
	prevY = y;	
}

void writeScore(int score){
	LCD_Goto(2,10);
	LCD_SetTextColor(255,255,240);	
	LCD_PrintInteger(score);
}

void eraseScore(void){
	LCD_Goto(2,10);
	LCD_SetTextColor(0,0,0);
	LCD_PrintInteger(99999999);
}

void writeHighscore(int score){
	LCD_Goto(2,16);
	LCD_SetTextColor(255,255,240);	
	LCD_PrintInteger(score);
}

void writeTime(int time){
	LCD_Goto(2,13);
	LCD_SetTextColor(255,255,240);
	LCD_PrintInteger(time);
}

void eraseTime(void){
	LCD_Goto(2,13);
	LCD_SetTextColor(0,0,0);
	LCD_PrintInteger(99999999);
}

int getValue(int tile){int row, col;
	row = (tile - 1) / 4;
	col = (tile - 1) % 4;
	return board[row][col];
}

void putValue(int value, int tile){int row, col;
	row = (tile - 1) / 4;
	col = (tile - 1) % 4;
	board[row][col] = value;
}

int tileNumber(int row, int col){
	return (row * 4) + 1 + col;
}

void clearDifferenceArray(void){int i;
	for (i=0; i<17; i++){
		positionDifference[i] = 0;
	}
}

void clearBoard(void){int row, col;
	for (row=0; row<4; row++){
		for (col=0; col<4; col++){
			board[row][col] = 0;
		}
	}
}

int boardTotal(void){int sum, tile;
	sum = 0;
	for (tile=1; tile<17; tile++){
		sum += getValue(tile);
	}
	return sum;
}

void drawAllTiles(void){int i, value;
	for (i=1; i<17; i++){
		// get value of tile
		value = getValue(i);
		if (value != 0){
			drawTile(value, i);
		}
	}
}

void displayHighestTile(void){int high, i, value;
	high = 2;
	for (i=1; i<17; i++){
		value = getValue(i);
		if (value > high){
			high = value;
		}
	}
	if (high == 2){
		LCD_DrawBMP(Number2, 26, 158);
	}
	else if (high == 4){
		LCD_DrawBMP(Number4, 26, 158);
	}
	else if (high == 8){
		LCD_DrawBMP(Number8, 26, 158);
	}
	else if (high == 16){
		LCD_DrawBMP(Number16, 26, 158);
	}
	else if (high == 32){
		LCD_DrawBMP(Number32, 26, 158);
	}
	else if (high == 64){
		LCD_DrawBMP(Number64, 26, 158);
	}
	else if (high == 128){
		LCD_DrawBMP(Number128, 26, 158);
	}
	else if (high == 256){
		LCD_DrawBMP(Number256, 26, 158);
	}
	else if (high == 512){
		LCD_DrawBMP(Number512, 26, 158);
	}
	else if (high == 1024){
		LCD_DrawBMP(Number1024, 26, 158);
	}
	else if (high == 2048){
		LCD_DrawBMP(Number2048, 26, 158);
	}	
}

void eraseBoard(void){
	LCD_DrawFilledRect(boardX,boardY,boardLen,boardLen,ORANGE);
}

void drawPauseMode(void){
	LCD_DrawFilledRect(boardX,boardY,boardLen,boardLen,BLACK);
	LCD_SetTextColor(255,255,240);	
	LCD_Goto(25,13);
	printf("Continue");
	LCD_Goto(38,13);
	printf("Restart");
	LCD_DrawRect(144,112,58,16,WHITE);
}

void shiftLeft(void){int done, row, col, sum, i;

	// clear difference array
	clearDifferenceArray();	
	
	// calculate amount of left shifts for each tile
	for (row=0; row<4; row++){
		for (col=1; col<4; col++){
			
			// shifts = empty tiles to left of tile
			sum = 0;
			for (i=0; i<col; i++){
				if (getValue(tileNumber(row, i)) == 0 
				&& getValue(tileNumber(row,col)) != 0){
					sum += 1;
				}
			}
			positionDifference[tileNumber(row,col)] = sum;
		}
	}	
	
	// shift each row
	for (row=0; row<4; row++){
		done = 0;
		
		// keep shifting while row isn't done shifting
		while (done == 0){
			done = 1;
			
			// check if row is shifted
			for (col=1; col<4; col++){
				// if any non-zero tile has a right zero neighbor, not shifted
				if ((board[row][col] != 0) && (board[row][col-1] == 0)){
					done = 0;
				}
			}
			
			// shift tiles if not done
			if (done == 0){
				for (col=1; col<4; col++){
					// if any non-zero tile has a right zero neighbor, shift to left
					if ((board[row][col] != 0) && (board[row][col-1] == 0)){
						// shift right
						board[row][col-1] = board[row][col];
						board[row][col] = 0;
					}
				}				
			}
			
		}
	}
}

void shiftRight(void){int done, row, col, sum, i;

	// clear difference array
	clearDifferenceArray();	
	
	// calculate amount of right shifts for each tile
	for (row=0; row<4; row++){
		for (col=0; col<3; col++){
			
			// shifts = empty tiles to right of tile
			sum = 0;
			for (i=col+1; i<4; i++){
				if (getValue(tileNumber(row, i)) == 0 
				&& getValue(tileNumber(row,col)) != 0){
					sum += 1;
				}
			}
			positionDifference[tileNumber(row,col)] = sum;
		}
	}	
	
	// shift each row
	for (row=0; row<4; row++){
		done = 0;
		
		// keep shifting while row isn't done shifting
		while (done == 0){
			done = 1;
			
			// check if row is shifted
			for (col=2; col>=0; col--){
				// if any non-zero tile has a right zero neighbor, not shifted
				if ((board[row][col] != 0) && (board[row][col+1] == 0)){
					done = 0;
				}
			}
			
			// shift tiles if not done
			if (done == 0){
				for (col=2; col>=0; col--){
					// if any non-zero tile has a right zero neighbor, shift to left
					if ((board[row][col] != 0) && (board[row][col+1] == 0)){
						// shift right
						board[row][col+1] = board[row][col];
						board[row][col] = 0;
					}
				}				
			}
			
		}
	}
}

void shiftUp(void){int done, row, col, sum, i;
	
	// clear difference array
	clearDifferenceArray();	
	
	// calculate amount of up shifts for each tile
	for (row=1; row<4; row++){
		for (col=0; col<4; col++){
			
			// shifts = empty tiles above tile
			sum = 0;
			for (i=0; i<row; i++){
				if (getValue(tileNumber(i,col)) == 0 
				&& getValue(tileNumber(row,col)) != 0){
					sum += 1;
				}
			}
			positionDifference[tileNumber(row,col)] = sum;
		}
	}		
	
	// shift each col
	for (col=0; col<4; col++){
		done = 0;
		
		// keep shifting while col isn't done shifting
		while (done == 0){
			done = 1;
			
			// check if col is shifted
			for (row=1; row<4; row++){
				// if any non-zero tile has an upper zero neighbor, not shifted
				if ((board[row][col] != 0) && (board[row-1][col] == 0)){
					done = 0;
				}
			}
			
			// shift tiles if not done
			if (done == 0){
				for (row=1; row<4; row++){
					// if any non-zero tile has an upper zero neighbor, shift up
					if ((board[row][col] != 0) && (board[row-1][col] == 0)){
						// shift up
						board[row-1][col] = board[row][col];
						board[row][col] = 0;
					}
				}				
			}
			
		}
	}
}

void shiftDown(void){int done, row, col, sum, i;
	
	// clear difference array
	clearDifferenceArray();	
	
	// calculate amount of down shifts for each tile
	for (row=0; row<3; row++){
		for (col=0; col<4; col++){
			
			// shifts = empty tiles below tile
			sum = 0;
			for (i=row+1; i<4; i++){
				if (getValue(tileNumber(i, col)) == 0 
				&& getValue(tileNumber(row,col)) != 0){
					sum += 1;
				}
			}
			positionDifference[tileNumber(row,col)] = sum;
		}
	}		
	
	// shift each col
	for (col=0; col<4; col++){
		done = 0;
		
		// keep shifting while col isn't done shifting
		while (done == 0){
			done = 1;
			
			// check if col is shifted
			for (row=2; row>=0; row--){
				// if any non-zero tile has a upper zero neighbor, not shifted
				if ((board[row][col] != 0) && (board[row+1][col] == 0)){
					done = 0;
				}
			}
			
			// shift tiles if not done
			if (done == 0){
				for (row=2; row>=0; row--){
					// if any non-zero tile has a upper zero neighbor, shift up
					if ((board[row][col] != 0) && (board[row+1][col] == 0)){
						// shift left
						board[row+1][col] = board[row][col];
						board[row][col] = 0;
					}
				}				
			}
			
		}
	}
}

int mergeLeft(void){int row, col, score;
	score = 0;
	for (row=0; row<4; row++){
		for (col=1; col<4; col++){
			if (board[row][col] == board[row][col-1]){
				board[row][col-1] *= 2;
				score += (board[row][col] * 2);
				board[row][col] = 0;
			}
		}
	}
	return score;
}

int mergeRight(void){int row, col, score;
	score = 0;
	for (row=0; row<4; row++){
		for (col=2; col>=0; col--){
			if (board[row][col] == board[row][col+1]){
				board[row][col+1] *= 2;
				score += (board[row][col] * 2);
				board[row][col] = 0;
			}
		}
	}
	return score;
}

int mergeUp(void){int row, col, score;
	score = 0;
	for (col=0; col<4; col++){
		for (row=1; row<4; row++){
			if (board[row-1][col] == board[row][col]){
				board[row-1][col] *= 2;
				score += (board[row][col] * 2);
				board[row][col] = 0;
			}
		}
	}
	return score;
}

int mergeDown(void){int row, col, score;
	score = 0;
	for (col=0; col<4; col++){
		for (row=2; row>=0; row--){
			if (board[row+1][col] == board[row][col]){
				board[row+1][col] *= 2;
				score += (board[row][col] * 2);
				board[row][col] = 0;
			}
		}
	}
	return score;
}

int countEmptyTiles(void){int sum, i;
	sum = 0;
	for (i=1; i<17; i++){
		if (getValue(i) == 0){
			sum += 1;
		}
	}
	return sum;
}

void generateRandomTile(void){ int i, x, sum, rand, tileNum, tileVal, tiles[16];
	
	// Generate random tile number
	
	// index of temporary tile array
	x = 0;
	
	// number of empty tiles
	sum = 0;
	
	// get empty tile numbers
	for (i=1; i<17; i++){
		if (getValue(i) == 0){
			tiles[x] = i;
			x += 1;
			sum += 1;
		}
	}

	// generate tile number from empty tiles
	tileNum = tiles[(Random()%sum)];
	
	//	Generate tile value (80% 2, 20% 4)
	rand = Random()%100;
	if (rand < 20){
		tileVal = 4;
	}
	else{
		tileVal = 2;
	}
	
	// put new tile on board
	putValue(tileVal, tileNum);
}

int checkGameOver(void){int row, col;
	
	// if theres still tiles remaining, game not over
	if (countEmptyTiles() != 0){
		return 0;
	}
	
	// if any two adjacent squares equal, game not over
	for (row=0; row<4; row++){
		for (col=0; col<3; col++){
			if (board[row][col] == board[row][col+1]){
				return 0;
			}
		}
	}
	for (col=0; col<4; col++){
		for (row=0; row<3; row++){
			if (board[row][col] == board[row+1][col]){
				return 0;
			}
		}
	}
	
	// else, game over
	return 1;
}

void drawGameOver(int score, int time){
	LCD_DrawFilledRect(boardX,boardY,boardLen,boardLen,BLACK);
	LCD_SetTextColor(255,255,240);	
	LCD_Goto(30,5);
	printf("GAME OVER");
	LCD_Goto(23,8);
	printf("Score:");
	LCD_Goto(29,8);
	LCD_PrintInteger(score);
	LCD_Goto(23,10);
	printf("Time:");
	LCD_Goto(28,10);
	LCD_PrintInteger(time);
	LCD_Goto(29,19);
	printf("Play Again?");
	LCD_DrawRect(170,166,72,16,WHITE);	
}

void gameOverHighscore(int score){
	LCD_Goto(23,12);
	printf("NEW HIGHSCORE!");
}


