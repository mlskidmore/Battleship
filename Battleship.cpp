// Battleship.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cstdlib>
#include <curses.h>
#include <fstream>
#include <cstring>
#include <cctype>
#include <time.h>

#include <cstdio>
#include <Windows.h>
#pragma execution_character_set("utf-8")

using namespace std;

struct scores
{
	int score;
	char inits[4];
};

void displayASCIIchars();
void initCurses();//function to start curses
void displaySplashScreen();//display
void displayInstructionScreen();
void playerSetup(char playerboard[10][10]);
void displayBoard(char board[10][10]);
void drawBoard(bool whichboard);
void compSetup(char compboard[10][10]);
void playerTurn(char compboard[10][10], bool& playerwin, int& CVN2, int& BB2, int& SSN2, int& DDG2, int& PT2);
void compTurn(char board[10][10], bool& compwin, int AI[6], int& CVN1, int& BB1, int& SSN1, int& DDG1, int& PT1);
void displayHit(bool who);
scores endGame(bool whowon, int guesses, int CVN1, int BB1, int SSN1, int DDG1, int PT1);
void getHighScores(scores highscores[5]);
void compareScores(scores highscores[5], scores score);
void printHighScores(scores highscores[9]);

void clrScreen();

int main()
{
	srand((unsigned int)time(NULL));
	scores score, highscores[5];
	
	char playerboard[10][10];//declare the player's board array
	char compboard[10][10];//declare the computer's board array

	char c = '\0';
	int CVN1 = 5, BB1 = 4, SSN1 = 3, DDG1 = 3, PT1 = 2;//player's ships
	int CVN2 = 5, BB2 = 4, SSN2 = 3, DDG2 = 3, PT2 = 2;//computer's ships

	// Variables associated with the unhit sections of each ship
	int compAI[6] = { 0, 0, 0, 0, 0, 0 };
	bool playerwin = false, compwin = false, whichboard = false;
	int guesses = 0;
	
	//displayASCIIchars();

	initCurses();//start curses
	displaySplashScreen();//display the splash screen
	displayInstructionScreen();

	playerSetup(playerboard);//Set up the player
	compSetup(compboard);//Set up the computer

	while (!playerwin && !compwin) {//play the game
		
		playerTurn(compboard, playerwin, CVN2, BB2, SSN2, DDG2, PT2);
		guesses++;

		if (playerwin) {
			score = endGame(1, guesses, CVN1, BB1, SSN1, DDG1, PT1);
			break;
		}

		compTurn(playerboard, compwin, compAI, CVN1, BB1, SSN1, DDG1, PT1);
		if (compwin)
			score = endGame(0, guesses, CVN1, BB1, SSN1, DDG1, PT1);
	}

	getHighScores(highscores);//read in initials and scores
	compareScores(highscores, score);//compare them
	printHighScores(highscores);//print to screen and file

	//getch(); 
	endwin(); // function that closes the curses window

	return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------
void displayASCIIchars()
{
	SetConsoleOutputCP(65001);

	for (int i = 0x2554; i <= 0x255D; i += 0x10)
	{
		for (int c = 0; c <= 0xF; ++c)
		{
			cout << (char)(i + c);
		}
	}
	cin.get();
}
//-----------------------------------------------------------------------------
void initCurses()
{
	(void)initscr();      /* initialize the curses library */
	keypad(stdscr, TRUE);  /* enable keyboard mapping */
	(void)nonl();         /* tell curses not to do NL->CR/NL on output */
	(void)cbreak();       /* take input chars one at a time, no wait for \n */
	(void)noecho();       /* don't echo input */
	start_color();
	init_color(11, 400, 400, 400);
	init_pair(1, COLOR_RED, 11);
	init_pair(2, COLOR_WHITE, COLOR_BLUE);
	init_pair(3, COLOR_WHITE, 11);
	init_pair(4, COLOR_WHITE, COLOR_RED);
	halfdelay(5);
	return;
}
//------------------------------------------------------------------------------
void displaySplashScreen() {
	mvaddstr(0, 1, "******     *    *******  *******  *      ");
	mvaddstr(1, 1, "*     *   * *      *        *     *      ");
	mvaddstr(2, 1, "*     *  *   *     *        *     *      ");
	mvaddstr(3, 1, "*     *  *   *     *        *     *      ");
	mvaddstr(4, 1, "******   *****     *        *     *      ");
	mvaddstr(5, 1, "*     *  *   *     *        *     *      ");
	mvaddstr(6, 1, "*     *  *   *     *        *     *      ");
	mvaddstr(7, 1, "*     *  *   *     *        *     *      ");
	mvaddstr(8, 1, "*     *  *   *     *        *     *      ");
	mvaddstr(9, 1, "******   *   *     *        *     ****** ");

	mvaddstr(0, 42, "*****   ****   *     * *******  *****  ");
	mvaddstr(1, 42, "*      *    *  *     *    *     *    * ");
	mvaddstr(2, 42, "*      *       *     *    *     *    * ");
	mvaddstr(3, 42, "*      *       *     *    *     *    * ");
	mvaddstr(4, 42, "*****   ****   *******    *     *****  ");
	mvaddstr(5, 42, "*           *  *     *    *     *      ");
	mvaddstr(6, 42, "*           *  *     *    *     *      ");
	mvaddstr(7, 42, "*           *  *     *    *     *      ");
	mvaddstr(8, 42, "*      *    *  *     *    *     *      ");
	mvaddstr(9, 42, "*****   ****   *     * *******  *      ");

	mvaddstr(11, 24, "The classic naval combat board game");

	for (int i = 0; i < 50; i++) {
		mvaddch(13, 15 + i, '-');
		mvaddch(22, 15 + i, '-');
	}

	mvaddstr(24, 29, "Press any key to continue");
	int c;

	for (int i = 0; i < 40; i++) {
		for (int y = 14; y < 22; y++)
			for (int x = 15; x < 76; x++)
				mvaddch(y, x, ' ');

		if (i >= 0 && i < 8)
			mvaddstr(21 - i, 19, "Based on the board game by Milton Bradley");
		if (i >= 2 && i < 10)
			mvaddstr(23 - i, 18, "Version by Mark Skidmore and Marvin Dennis");
		if (i >= 4 && i < 12)
			mvaddstr(25 - i, 31, "Head Programmer");
		if (i >= 5 && i < 13)
			mvaddstr(26 - i, 31, "---------------");
		if (i >= 6 && i < 14)
			mvaddstr(27 - i, 32, "Marvin Dennis");
		if (i >= 8 && i < 16)
			mvaddstr(29 - i, 33, "Programmers");
		if (i >= 9 && i < 17)
			mvaddstr(30 - i, 32, "-------------");
		if (i >= 10 && i < 18)
			mvaddstr(31 - i, 32, "Mark Skidmore");
		if (i >= 11 && i < 19)
			mvaddstr(32 - i, 32, "Marvin Dennis");
		if (i >= 13 && i < 21)
			mvaddstr(34 - i, 32, "Lead Designer");
		if (i >= 14 && i < 22)
			mvaddstr(35 - i, 32, "-------------");
		if (i >= 15 && i < 23)
			mvaddstr(36 - i, 32, "Marvin Dennis");
		if (i >= 17 && i < 25)
			mvaddstr(38 - i, 32, "Visual Effects");
		if (i >= 18 && i < 26)
			mvaddstr(39 - i, 32, "--------------");
		if (i >= 19 && i < 27)
			mvaddstr(40 - i, 32, "Marvin Dennis");
		if (i >= 21 && i < 29)
			mvaddstr(42 - i, 34, "Producers");
		if (i >= 22 && i < 30)
			mvaddstr(43 - i, 32, "-------------");
		if (i >= 23 && i < 31)
			mvaddstr(44 - i, 32, "Mark Skidmore");
		if (i >= 24 && i < 32)
			mvaddstr(45 - i, 32, "Marvin Dennis");
		if (i >= 26 && i < 34)
			mvaddstr(47 - i, 30, "Special thanks to");
		if (i >= 27 && i < 35)
			mvaddstr(48 - i, 30, "-----------------");
		if (i >= 28 && i < 36)
			mvaddstr(49 - i, 32, "Sushil Louis");
		if (i >= 30 && i < 37)
			mvaddstr(50 - i, 32, "Leandro Loss");
		if (i >= 32 && i < 39)
			mvaddstr(52 - i, 31, "Enjoy the game");
		c = getch();
		if (c != -1) break;
	}
	nocbreak();
	cbreak();
	clrScreen();
}
//------------------------------------------------------------------------------
void displayInstructionScreen()
{
	mvaddstr( 7, 24, "Welcome to Battleship.");
	mvaddstr( 9, 24, "Use the Up, Down, Left & Right arrow keys");
	mvaddstr(10, 24, "to move your ships on the board.");
	mvaddstr(12, 24, "Use the space bar to orient each ship");
	mvaddstr(13, 24, "vertically or horizontally.");
	mvaddstr(15, 24, "Use the Enter key to place your ship.");
	mvaddstr(24, 24, "Press any key to continue");
	getch();
	clrScreen();
}
//------------------------------------------------------------------------------
void playerSetup(char playerboard[10][10])
{
	bool hv, placed, alreadysub = false;//hv is horizontal or vertical
	int c, ok;//c is used to hold user's input
	char s;//used to hold character representing type of ship

	drawBoard(0);//draw the player's board on the screen (player = 0)

	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
			playerboard[i][j] = ' ';//initialize the "virtual board" with spaces

	for (int i = 5; i > 1; i--) {// open for 1
		
		int y = 0;//row reference position upper lh corner of player's board
		int x = 0;//reference position upper rh corner of player's board
		placed = false;

		if (i == 5) s = 'A';//aircraft carrier
		if (i == 4) s = 'B';//battleship
		if (i == 3 && !alreadysub) s = 'S';//sub
		if (i == 3 && alreadysub) s = 'D';//destroyer
		if (i == 2) s = 'P';//PT boat
		hv = true;//vertical is true

		for (int j = 0; j < i; j++)
			mvaddch(y * 2 + 4, (x + j) * 2 + 7, s); //init. place each ship on the screen

		while (true) { //open while
			ok = 0;
			c = getch();//start getting user's input
			for (int j = 0; j < i; j++) { //open for 2
				if (!hv)//if the ship is vertical...
					mvaddch((y + j) * 2 + 4, x * 2 + 7, ' ');// Erase old ship
				if (hv)//if the ship is horizontal...
					mvaddch(y * 2 + 4, (x + j) * 2 + 7, ' ');// Erase old ship
			} // close for 2
			switch (c)
			{//start switch

			case 258://down arrow
				y++;
				break;

			case 259://up arrow
				y--;
				break;

			case 260://left arrow
				x--;
				break;

			case 261://right arrow
				x++;
				break;

			case ' '://by hitting the space bar, changes orient. h/v to v/h
				if (hv == 1) hv = 0;
				else if (hv == 0) hv = 1;
				break;

			case 13://hitting return places the ship into the array
				if (!hv) {//if the ship is vertical...
					for (int j = 0; j < i; j++)
						if (playerboard[y + j][x] == ' ') ok++;

					if (ok == i) {
						for (int j = 0; j < i; j++)
							playerboard[y + j][x] = s;
						placed = true;
						break;
					}
				}
				if (hv) {//if the ship is horizontal
					for (int j = 0; j < i; j++)
						if (playerboard[y][x + j] == ' ') ok++;
					if (ok == i) {
						for (int j = 0; j < i; j++)
							playerboard[y][x + j] = s;
						placed = true;
						break;
					}
				}
			default:
				break;
			}//end switch
			//control the placement of each ship; don't overlay or go off brd
			if (x < 0) x = 0;
			if (y < 0) y = 0;
			if (x > 9) x = 9;
			if (hv && x > 10 - i) x = 10 - i;
			if (y > 9) y = 9;
			if (!hv && y > 10 - i) y = 10 - i;

			displayBoard(playerboard);
			if (placed) break;
			for (int j = 0; j < i; j++)
			{// open for 3
				if (!hv)
					mvaddch((y + j) * 2 + 4, x * 2 + 7, s);
				if (hv)
					mvaddch(y * 2 + 4, (x + j) * 2 + 7, s);
			}// close for 3                

		} //close while
		if (i == 3 && !alreadysub) {
			alreadysub = true;
			i++;
		}
	}// close for 1

	return;
}// end function
//------------------------------------------------------------------------------
void drawBoard(bool whichboard)
{//open drawBoard function
 //print game headers
	attron(COLOR_PAIR(2));
	if (whichboard)
		mvaddstr(0, 56, "COMPUTER'S BOARD");
	else mvaddstr(0, 9, "PLAYER'S BOARD");

	//Label columns 0 through 9
	int x = 0;//initialize vertical incrementer
	for (char n = '0'; n <= '9'; n++)//starting with number 0, increment thru 9...  
		//place each number in column 9 on appropriate row
		mvaddch(2, 7 + whichboard * 47 + 2 * x++, n);

	//build the corners and sides of the game board 
	for (int y = 2; y < 21; y++)//start with the corners
	{//open for loop 1
		mvaddch(3, whichboard * 47 + 6, ACS_ULCORNER); //ASCII char for top    left corner
		mvaddch(3, whichboard * 47 + 26, ACS_URCORNER); //ASCII char for top    right corner
		mvaddch(23, whichboard * 47 + 6, ACS_LLCORNER); //ASCII char for bottom left corner
		mvaddch(23, whichboard * 47 + 26, ACS_LRCORNER); //ASCII char for bottom right corner

		if (y % 2 == 0)//left side of the game board
			mvaddch(y + 2, whichboard * 47 + 6, ACS_VLINE);//dble brder ASCII char for l side
		else mvaddch(y + 2, whichboard * 47 + 6, ACS_VLINE);//dble border w/tick ASCII char for lft side

		if (y % 2 == 0)//right side of the game board
			mvaddch(y + 2, whichboard * 47 + 26, ACS_VLINE);//dble brdr ASCII char for rt side
		else mvaddch(y + 2, whichboard * 47 + 26, ACS_VLINE);//dble border w/tick ASCII char for rt side

		for (int x = 7; x < 26; x++)//inner vertical column separators
		{
			if (x % 2 == 0)
				mvaddch(y + 2, whichboard * 47 + x, ACS_VLINE);//ASCII char for a verticle bar
			if (x % 2 != 0)
				mvaddch(y + 2, whichboard * 47 + x, ' '); // fill blank game board squares with color
		}
	}//close for loop 1

	//build the top, inner and bottom horizontal sections of the game board
	for (int y = 0; y < 22; y++)//21 horizontal sections
	{//open for loop 2
		for (int x = 7; x < 26; x++)//21 veritical positions
		{//open for loop 3
			if (y == 0)//top horizontal section
			{
				if (x % 2 == 0)//even vertical separators
					mvaddch(2 * y + 3, whichboard * 47 + x, ACS_HLINE);//ASCII code 209 for top cols
				else mvaddch(2 * y + 3, whichboard * 47 + x, ACS_HLINE);//ASCII code 205 for top non-cols
			}

			if (y > 0 && y < 11)//inner horizontal sections
			{
				if (x % 2 == 0)//even columns cross (intersection) pieces
					mvaddch(2 * y + 3, whichboard * 47 + x, ACS_HLINE);
				else mvaddch(2 * y + 3, whichboard * 47 + x, ACS_HLINE);//odd cols horiz pieces
			}

			else if (y == 21)//bottom horizontal section
			{
				if (x % 2 == 0)
					mvaddch(y + 2, whichboard * 47 + x, ACS_HLINE);//ASCII code 207 for inner
														 //horiz borders
				else mvaddch(y + 2, whichboard * 47 + x, ACS_HLINE);//ASCII code 205 for
			}                                          //top non-columns
		}//close for loop 3
	}//close for loop 2

 //Label rows A through J
	int y = 0;//initialize horizontal incrementer
	for (char c = 'A'; c <= 'J'; c++)//starting with letter A, increment thru J...
		mvaddch(4 + 2 * y++, 5 + whichboard * 47, c);//put each letter on row 2 in appropriate column

}//close drawBoard function
//------------------------------------------------------------------------------
void displayBoard(char board[10][10])//display player's ships on screen
{//open function
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
		{//open inner for
			if (board[i][j] == ' ') mvaddch(4 + 2 * i, 7 + 2 * j, ' ');
			else
			{//open else
				attron(COLOR_PAIR(1));
				mvaddch(4 + 2 * i, 7 + 2 * j, board[i][j]);

				if (board[i][j] == board[i + 1][j])
					mvaddch(5 + 2 * i, 7 + 2 * j, ' ');

				if (board[i][j] == board[i][j + 1])
					mvaddch(4 + 2 * i, 8 + 2 * j, ' ');

				attron(COLOR_PAIR(2));
			} //close else
		} //close inner for
}//close function
//------------------------------------------------------------------------------
void compSetup(char board[10][10]) {
	bool hv, alreadysub = false;
	int y, x, ok;
	char c;

	drawBoard(1);
	
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
			board[i][j] = ' ';
	
	for (int i = 5; i > 1; i--) {
		while (true) {
			hv = rand() % 2;
			ok = 0;
			if (i == 5) c = 'A';
			if (i == 4) c = 'B';
			if (i == 3 && !alreadysub) c = 'S';
			if (i == 3 && alreadysub) c = 'D';
			if (i == 2) c = 'P';

			if (!hv) {
				y = rand() % (11 - i);
				x = rand() % 10;
				for (int j = 0; j < i; j++)
					if (board[y + j][x] == ' ') ok++;
				if (ok == i) {
					for (int j = 0; j < i; j++) board[y + j][x] = c;
					break;
				}
			}
			if (hv) {
				x = rand() % (11 - i);
				y = rand() % 10;
				for (int j = 0; j < i; j++)
					if (board[y][x + j] == ' ') ok++;
				if (ok == i) {
					for (int j = 0; j < i; j++) board[y][x + j] = c;
					break;
				}
			}
		}
		if (i == 3 && !alreadysub) {
			alreadysub = true;
			i++;
		}
	}
}
//------------------------------------------------------------------------------
void playerTurn(char compboard[10][10], bool& playerwin, int& CVN2, int& BB2, int& SSN2, int& DDG2, int& PT2)
{//open function

	int row, col;
	attroff(COLOR_PAIR(2));//turn colors off while accepting input

	do {//open do 1
		mvaddstr(03, 28, "Enter Row: ");
		move(03, 42);

		do
		{//open do 2
			row = getch();//get player's row guess
			row = toupper(row);//convert it to upper case
		}//close do 2

		while ((row < 65 || row > 74));// && (row < 97 && row > 106));
			mvaddch(03, 42, row);//display player's row guess

		mvaddstr(04, 28, "Enter Column: ");

		do
		{//open do 3
			col = getch();//get player's column guess
		}//close do 3

		while ((col < 48 || col > 57));
		mvaddch(04, 42, col);//display user's column guess

		row -= 65;//convert row input from int to char
		col -= 48;//convert col input from int to char

		if (compboard[row][col] == '*' || compboard[row][col] == 'X')
		{
			mvaddstr(6, 28, "You already guessed");
			mvaddstr(7, 28, "there. Fire again.");
		}
	}//close do 1

	while (compboard[row][col] == '*' || compboard[row][col] == 'X');

	attron(COLOR_PAIR(2));//turn colors back on

	switch (compboard[row][col])
	{//open switch

	case ' ':
		compboard[row][col] = 'X';
		mvaddch(4 + 2 * row, 54 + 2 * col, 42);
		break;
	case 'A':
		compboard[row][col] = '*';
		CVN2--;
		attron(COLOR_PAIR(1));//turn off white/blue; turn on red/gry
		mvaddch(4 + 2 * row, 54 + 2 * col, 42);
		attron(COLOR_PAIR(2));//turn white/blue back on
		displayHit(0);
		break;
	case 'B':
		compboard[row][col] = '*';
		BB2--;
		attron(COLOR_PAIR(1));
		mvaddch(4 + 2 * row, 54 + 2 * col, 42);
		attron(COLOR_PAIR(2));
		displayHit(0);
		break;
	case 'D':
		compboard[row][col] = '*';
		DDG2--;
		attron(COLOR_PAIR(1));
		mvaddch(4 + 2 * row, 54 + 2 * col, 42);
		attron(COLOR_PAIR(2));
		displayHit(0);
		break;
	case 'P':
		compboard[row][col] = '*';
		PT2--;
		attron(COLOR_PAIR(1));
		mvaddch(4 + 2 * row, 54 + 2 * col, 42);
		attron(COLOR_PAIR(2));
		displayHit(0);
		break;
	case 'S':
		compboard[row][col] = '*';
		SSN2--;
		attron(COLOR_PAIR(1));
		mvaddch(4 + 2 * row, 54 + 2 * col, 42);
		attron(COLOR_PAIR(2));
		displayHit(0);
		break;
	default:
		break;
	}//close switch

	attroff(COLOR_PAIR(2));
	mvaddstr(6, 28, "                   ");//erase screen line 6 from above
	mvaddstr(7, 28, "                  ");//erase screen line 7 from above

	for (int y = 18; y < 22; y++)//erase sunk ship message
		for (int x = 28; x < 52; x++)
			mvaddch(y, x, ' ');
	if (CVN2 == 0) {
		mvaddstr(20, 28, "You sunk your enemy's");
		mvaddstr(21, 28, "Aircraft Carrier");
		CVN2 = -1;
	}
	if (BB2 == 0) {
		mvaddstr(20, 28, "You sunk your enemy's");
		mvaddstr(21, 28, "Battleship");
		BB2 = -1;
	}
	if (SSN2 == 0) {
		mvaddstr(20, 28, "You sunk your enemy's");
		mvaddstr(21, 28, "Submarine");
		SSN2 = -1;
	}
	if (DDG2 == 0) {
		mvaddstr(20, 28, "You sunk your enemy's");
		mvaddstr(21, 28, "Destroyer");
		DDG2 = -1;
	}
	if (PT2 == 0) {
		mvaddstr(20, 28, "You sunk your enemy's");
		mvaddstr(21, 28, "PT Boat");
		PT2 = -1;
	}
	attron(COLOR_PAIR(2));

	if (CVN2 == -1 && BB2 == -1 && SSN2 == -1 && DDG2 == -1 && PT2 == -1)
		playerwin = true;

}//close function
//------------------------------------------------------------------------------
void compTurn(char board[10][10], bool& compwin, int AI[6], int& CVN, int& BB,
	int& SSN, int& DDG2, int& PT) {

	int x, y, udlr;
	bool hv = false, reversed = false, presumedsunk = false, hitmiss = false;

	if (AI[0] == 0) {
		do
		{
			x = rand() % 10;
			y = rand() % 10;
		}
		while (board[y][x] == 'X' || board[y][x] == '*');

		switch (board[y][x]) {
		case ' ':
			board[y][x] = 'X';
			mvaddch(4 + 2 * y, 7 + 2 * x, 42);
			break;
		case 'A':
			board[y][x] = '*';
			CVN--;
			attron(COLOR_PAIR(1));
			mvaddch(4 + 2 * y, 7 + 2 * x, 42);
			attron(COLOR_PAIR(2));
			AI[0] = 1;
			AI[2] = y;
			AI[3] = x;
			displayHit(1);
			break;
		case 'B':
			board[y][x] = '*';
			BB--;
			attron(COLOR_PAIR(1));
			mvaddch(4 + 2 * y, 7 + 2 * x, 42);
			attron(COLOR_PAIR(2));
			AI[0] = 1;
			AI[2] = y;
			AI[3] = x;
			displayHit(1);
			break;
		case 'S':
			board[y][x] = '*';
			SSN--;
			attron(COLOR_PAIR(1));
			mvaddch(4 + 2 * y, 7 + 2 * x, 42);
			attron(COLOR_PAIR(2));
			AI[0] = 1;
			AI[2] = y;
			AI[3] = x;
			displayHit(1);
			break;
		case 'D':
			board[y][x] = '*';
			DDG2--;
			attron(COLOR_PAIR(1));
			mvaddch(4 + 2 * y, 7 + 2 * x, 42);
			attron(COLOR_PAIR(2));
			AI[0] = 1;
			AI[2] = y;
			AI[3] = x;
			displayHit(1);
			break;
		case 'P':
			board[y][x] = '*';
			PT--;
			attron(COLOR_PAIR(1));
			mvaddch(4 + 2 * y, 7 + 2 * x, 42);
			attron(COLOR_PAIR(2));
			AI[0] = 1;
			AI[2] = y;
			AI[3] = x;
			displayHit(1);
			break;
		}
	}
	else {
		if (AI[1] == 0) hv = rand() % 2;
		if (AI[1] == 1) hv = 0;
		if (AI[1] == 2) hv = 1;
		y = AI[2];
		x = AI[3];
		udlr = (rand() % 2) * 2 - 1;
		if (hv == 0) {
			while (!hitmiss) {
				x += udlr;
				if (x < 0) {
					x = 1;
					udlr = 1;
					if (AI[4] == 0)
						AI[4] = 1;
					if (AI[4] == 1)
						AI[4] = 0;
				}
				if (x > 10) {
					x = 9;
					udlr = -1;
					if (AI[4] == 0)
						AI[4] = 1;
					if (AI[4] == 1)
						AI[4] = 0;
				}
				switch (board[y][x]) {
				case ' ':
					board[y][x] = 'X';
					mvaddch(4 + 2 * y, 7 + 2 * x, 42);
					if (AI[4] == 0) {
						AI[4] = 1;
						AI[1] = 1;
						hitmiss = 1;
					}
					if (AI[4] == 1) {
						AI[4] = 0;
						mvaddch(4 + 2 * y, 7 + 2 * x, 42);
						AI[1] = 2;
						AI[5] = 1;
						hitmiss = 1;
					}
					break;
				case 'A':
					board[y][x] = '*';
					CVN--;
					attron(COLOR_PAIR(1));
					mvaddch(4 + 2 * y, 7 + 2 * x, 42);
					attron(COLOR_PAIR(2));
					AI[1] = 1;
					AI[5] = 1;
					hitmiss = 1;
					displayHit(1);
					break;
				case 'B':
					board[y][x] = '*';
					BB--;
					attron(COLOR_PAIR(1));
					mvaddch(4 + 2 * y, 7 + 2 * x, 42);
					attron(COLOR_PAIR(2));
					AI[1] = 1;
					AI[5] = 1;
					hitmiss = 1;
					displayHit(1);
					break;
				case 'S':
					board[y][x] = '*';
					SSN--;
					attron(COLOR_PAIR(1));
					mvaddch(4 + 2 * y, 7 + 2 * x, 42);
					attron(COLOR_PAIR(2));
					AI[1] = 1;
					AI[5] = 1;
					hitmiss = 1;
					displayHit(1);
					break;
				case 'D':
					board[y][x] = '*';
					DDG2--;
					attron(COLOR_PAIR(1));
					mvaddch(4 + 2 * y, 7 + 2 * x, 42);
					attron(COLOR_PAIR(2));
					AI[1] = 1;
					AI[5] = 1;
					hitmiss = 1;
					displayHit(1);
					break;
				case 'P':
					board[y][x] = '*';
					PT--;
					attron(COLOR_PAIR(1));
					mvaddch(4 + 2 * y, 7 + 2 * x, 42);
					attron(COLOR_PAIR(2));
					AI[1] = 1;
					AI[5] = 1;
					hitmiss = 1;
					displayHit(1);
					break;
				case 'X':
					if (AI[4] == 1) {
						AI[1] = 1;
						AI[4] = 0;
					}
					else {
						if (!reversed) {
							reversed = true;
							udlr *= -1;
						}
						else if (AI[5] == 0) hv = 1;
						else  presumedsunk = true;
					}

				}
				if (presumedsunk) {
					for (int i = 0; i < 6; i++)
						AI[i] = 0;
					break;
				}

			}
		}
		if (hv == 1) {
			while (!hitmiss) {
				y += udlr;
				if (y < 0) {
					y = 1;
					udlr = 1;
					if (AI[4] == 0)
						AI[4] = 1;
					if (AI[4] == 1)
						AI[4] = 0;
				}
				if (y > 10) {
					y = 9;
					udlr = -1;
					if (AI[4] == 0)
						AI[4] = 1;
					if (AI[4] == 1)
						AI[4] = 0;
				}
				switch (board[y][x]) {
				case ' ':
					board[y][x] = 'X';
					mvaddch(4 + 2 * y, 7 + 2 * x, 42);
					if (AI[4] == 0) {
						AI[4] = 1;
						AI[1] = 2;
						hitmiss = 1;
					}
					if (AI[4] == 1) {
						AI[4] = 0;
						mvaddch(4 + 2 * y, 7 + 2 * x, 42);
						AI[1] = 1;
						AI[5] = 1;
						hitmiss = 1;
					}
					break;
				case 'A':
					board[y][x] = '*';
					CVN--;
					attron(COLOR_PAIR(1));
					mvaddch(4 + 2 * y, 7 + 2 * x, 42);
					attron(COLOR_PAIR(2));
					AI[1] = 2;
					AI[5] = 1;
					hitmiss = 1;
					displayHit(1);
					break;
				case 'B':
					board[y][x] = '*';
					BB--;
					attron(COLOR_PAIR(1));
					mvaddch(4 + 2 * y, 7 + 2 * x, 42);
					attron(COLOR_PAIR(2));
					AI[1] = 2;
					AI[5] = 1;
					hitmiss = 1;
					displayHit(1);
					break;
				case 'S':
					board[y][x] = '*';
					SSN--;
					attron(COLOR_PAIR(1));
					mvaddch(4 + 2 * y, 7 + 2 * x, 42);
					attron(COLOR_PAIR(2));
					AI[1] = 2;
					AI[5] = 1;
					hitmiss = 1;
					displayHit(1);
					break;
				case 'D':
					board[y][x] = '*';
					DDG2--;
					attron(COLOR_PAIR(1));
					mvaddch(4 + 2 * y, 7 + 2 * x, 42);
					attron(COLOR_PAIR(2));
					AI[1] = 2;
					AI[5] = 1;
					hitmiss = 1;
					displayHit(1);
					break;
				case 'P':
					board[y][x] = '*';
					PT--;
					attron(COLOR_PAIR(1));
					mvaddch(4 + 2 * y, 7 + 2 * x, 42);
					attron(COLOR_PAIR(2));
					AI[1] = 2;
					AI[5] = 1;
					hitmiss = 1;
					displayHit(1);
					break;
				case 'X':
					if (AI[4] == 1) {
						AI[1] = 2;
						AI[4] = 0;
					}
					else {
						if (!reversed) {
							reversed = true;
							udlr *= -1;
						}
						else if (AI[5] == 0) hv = 0;
						else presumedsunk = true;
					}
				}
				if (presumedsunk) {
					for (int i = 0; i < 6; i++) AI[i] = 0;
					do {
						x = rand() % 10;
						y = rand() % 10;
					} while (board[y][x] == 'X' || board[y][x] == '*');
					switch (board[y][x]) {
					case ' ':
						board[y][x] = 'X';
						mvaddch(4 + 2 * y, 7 + 2 * x, 42);
						break;
					case 'A':
						board[y][x] = '*';
						CVN--;
						attron(COLOR_PAIR(1));
						mvaddch(4 + 2 * y, 7 + 2 * x, 42);
						attron(COLOR_PAIR(2));
						AI[0] = 1;
						AI[2] = y;
						AI[3] = x;
						displayHit(1);
						break;
					case 'B':
						board[y][x] = '*';
						BB--;
						attron(COLOR_PAIR(1));
						mvaddch(4 + 2 * y, 7 + 2 * x, 42);
						attron(COLOR_PAIR(2));
						AI[0] = 1;
						AI[2] = y;
						AI[3] = x;
						displayHit(1);
						break;
					case 'S':
						board[y][x] = '*';
						SSN--;
						attron(COLOR_PAIR(1));
						mvaddch(4 + 2 * y, 7 + 2 * x, 42);
						attron(COLOR_PAIR(2));
						AI[0] = 1;
						AI[2] = y;
						AI[3] = x;
						displayHit(1);
						break;
					case 'D':
						board[y][x] = '*';
						DDG2--;
						attron(COLOR_PAIR(1));
						mvaddch(4 + 2 * y, 7 + 2 * x, 42);
						attron(COLOR_PAIR(2));
						AI[0] = 1;
						AI[2] = y;
						AI[3] = x;
						displayHit(1);
						break;
					case 'P':
						board[y][x] = '*';
						PT--;
						attron(COLOR_PAIR(1));
						mvaddch(4 + 2 * y, 7 + 2 * x, 42);
						attron(COLOR_PAIR(2));
						AI[0] = 1;
						AI[2] = y;
						AI[3] = x;
						displayHit(1);
						break;
					}

					break;
				}
			}
		}
	}
	attroff(COLOR_PAIR(2));
	if (CVN == 0) {
		mvaddstr(18, 28, "The enemy sunk your");
		mvaddstr(19, 28, "Aircraft Carrier");
		CVN = -1;
		for (int i = 0; i < 6; i++) AI[i] = 0;
	}
	if (BB == 0) {
		mvaddstr(18, 28, "The enemy sunk your");
		mvaddstr(19, 28, "Battleship");
		BB = -1;
		for (int i = 0; i < 6; i++) AI[i] = 0;
	}
	if (SSN == 0) {
		mvaddstr(18, 28, "The enemy sunk your");
		mvaddstr(19, 28, "Submarine");
		SSN = -1;
		for (int i = 0; i < 6; i++) AI[i] = 0;
	}
	if (DDG2 == 0) {
		mvaddstr(18, 28, "The enemy sunk your");
		mvaddstr(19, 28, "Destroyer");
		DDG2 = -1;
		for (int i = 0; i < 6; i++) AI[i] = 0;
	}
	if (PT == 0) {
		mvaddstr(18, 28, "The enemy sunk your");
		mvaddstr(19, 28, "PT Boat");
		PT = -1;
		for (int i = 0; i < 6; i++) AI[i] = 0;
	}

	attron(COLOR_PAIR(2));
	if (CVN == -1 && BB == -1 && SSN == -1 && DDG2 == -1 && PT == -1)
		compwin = true;
}
//------------------------------------------------------------------------------
void displayHit(bool who)//display Hit message
{
	halfdelay(1);
	for (int i = 0; i < 7; i++)
	{
		attron(COLOR_PAIR(4));
		if (!who)
			mvaddstr(11, 32, "ENEMY HIT!");
		else mvaddstr(13, 32, "YOU GOT HIT!");

		getch();
		attroff(COLOR_PAIR(4));

		if (!who)
			mvaddstr(11, 32, "            ");
		else mvaddstr(13, 32, "            ");

		getch();
	}
	nocbreak();
	cbreak();
}
//------------------------------------------------------------------------------

scores endGame(bool whowon, int guesses, int CVN1, int BB1, int SSN1, int DDG1,
	int PT1) {
	scores score;
	int ships = 0;
	attroff(COLOR_PAIR(2));
	clrScreen();
	char scoreprint[5];
	if (CVN1 != -1)ships++;
	if (BB1 != -1)ships++;
	if (SSN1 != -1)ships++;
	if (DDG1 != -1)ships++;
	if (PT1 != -1)ships++;

	score.score = ships * 5 - guesses + 200 * whowon;
	sprintf_s(scoreprint, "%i", score.score);

	if (whowon)
		mvaddstr(10, 27, "Congratulations! You Won!");

	if (!whowon)
		mvaddstr(10, 20, "Sorry, you lose. Better luck next time");

	mvaddstr(13, 32, "Your score was");

	for (int i = 0; scoreprint[i] != '\0'; i++)
		mvaddch(14, 38 + i, scoreprint[i]);

	mvaddstr(16, 30, "Enter your initials.");
	move(17, 38);

	for (int n = 0; n < 3; n++)
	{//open inner for
		score.inits[n] = getch();
		score.inits[n] = toupper(score.inits[n]);
		mvaddch(17, 38 + n, score.inits[n]);
	}//close inner for
	score.inits[3] = '\0';
	mvaddstr(20, 27, "Press any key to continue");
	getch();
	clrScreen();
	return score;
}
//------------------------------------------------------------------------------
void getHighScores(scores highscores[5])
{//open getHighScores, read from top_scores.txt file
	ifstream fin;
	fin.open("highscores.txt");
	if (!fin) {
		endwin();
		system("PAUSE");
	}
	for (int i = 0; i < 5; i++) {//store inits and scores
		fin >> highscores[i].inits >> highscores[i].score;
	}
	fin.close();
}
//------------------------------------------------------------------------------
void compareScores(scores highscores[5], scores score)
{//open compareScores
	for (int i = 0; i < 5; i++)
	{//open outer for
		if (score.score > highscores[i].score)
		{//open if
			mvaddstr(10, 35, "Amazing!");
			mvaddstr(11, 29, "You got a high score!");
			mvaddstr(13, 27, "Press any key to continue");
			getch();

			for (int j = 5; j > 0; j--)
				highscores[i + j] = highscores[i + j - 1];

			highscores[i] = score;

			break;
		}//close if
	}//close outer for

	clrScreen();

}//close compareScores
//------------------------------------------------------------------------------
void printHighScores(scores highscores[9])
{
	char printscore[5];
	mvaddstr(1, 34, "High Scores");
	mvaddstr(2, 32, "-------------");
	for (int i = 0; i < 5; i++)
	{
		mvaddstr(2 * i + 4, 32, highscores[i].inits);
		sprintf_s(printscore, "%i", highscores[i].score);
		for (int j = 0; printscore[j] != '\0'; j++)
			mvaddch(2 * i + 4, 38 + j, printscore[j]);
	}
	mvaddstr(18, 25, "Game over. Thanks for playing.");
	mvaddstr(20, 29, "Press any key to exit.");
	getch();
	ofstream fout;
	fout.open("highscores.txt");
	for (int i = 0; i < 5; i++)
		fout << highscores[i].inits << " " << highscores[i].score << endl;
	fout.close();
}
//------------------------------------------------------------------------------   
void clrScreen()
{
	for (int y = 0; y < 25; y++)
		for (int x = 0; x < 80; x++)
			mvaddch(y, x, ' ');
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
