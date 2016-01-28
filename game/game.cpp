//  Filename: game.cpp
//  Created by Ivan Jonathan Hoo
//  Copyright (c) 2016 Ivan Jonathan Hoo. All rights reserved.

#include <GL/freeglut.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;


/*****************************Basic Drawing Functions*****************************/

//draw a clear canvas with specified color
void drawClear(const float color[4])
{
	glClearColor(color[0], color[1], color[2], color[3]);
	glClear(GL_COLOR_BUFFER_BIT);
}

//draw a line connnected by points (x1,y1) and (x2,y2) with specified color
void drawLine(float x1, float y1, float x2, float y2, const float color[4])
{
	glColor4fv(color);
	glLineWidth(4.0f);
	glBegin(GL_LINES);
		glVertex2f(x1, y1);
		glVertex2f(x2, y2);
	glEnd();
}

//draw a triangle connnected by points (x1,y1), (x2,y2) and (x3,y3) with specified color
void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, const float color[4])
{
	glColor4fv(color);
	glBegin(GL_TRIANGLES);
		glVertex2f(x1, y1);
		glVertex2f(x2, y2);
		glVertex2f(x3, y3);
	glEnd();
}

//draw a rectangle connected by points (x1,y1) and (x2,y2)
void drawRectangle(float x1, float y1, float x2, float y2, const float color[4])
{
	glColor4fv(color);
	glRectf(x1, y1, x2, y2);
}

//draw a square at center (centerX,centerY)
void drawSquare(float centerX, float centerY, const float color[4])
{
	drawRectangle(centerX-2.0f, centerY-2.0f, centerX+2.0f, centerY+2.0f, color);
}

//draw a point at (x,y)
void drawPoint(float x, float y, const float color[4])
{
	glColor4fv(color);
	glBegin(GL_POINTS);
		glVertex2f(x, y);
	glEnd();
}

/*********************************************************************************/


//board colorCode definitions
#define NO_TILE     0
#define RED_TILE    1
#define ORANGE_TILE 2
#define YELLOW_TILE 3
#define GREEN_TILE  4
#define BLUE_TILE   5
#define INDIGO_TILE 6
#define VIOLET_TILE 7
#define WHITE_TILE  8
//direction definitions
#define UP    0
#define RIGHT 1
#define LEFT  2
#define DOWN  3


//rainbow colors
const float RED[4]    = {1.0f, 0.0f, 0.0f, 1.0f};
const float ORANGE[4] = {1.0f, 165/255.0f, 0.0f, 1.0f};
const float YELLOW[4] = {1.0f, 1.0f, 0.0f, 1.0f};
const float GREEN[4]  = {0.0f, 1.0f, 0.0f, 1.0f};
const float BLUE[4]   = {0.0f, 0.0f, 1.0f, 1.0f};
const float INDIGO[4] = {75/255.0f, 0.0f, 130/255.0f, 1.0};
const float VIOLET[4] = {148/255.0f, 0.0f, 211/255.0f, 1.0};
//other colors
const float BACKGROUND_COLOR[4] = {0.2f, 0.2f, 0.2f, 1.0f};
const float GRID_COLOR[4]       = {0.8f, 0.8f, 0.8f, 1.0f};
const float BLACK[4]            = {0.0f, 0.0f, 0.0f, 1.0f};
const float WHITE[4]            = {1.0f, 1.0f, 1.0f, 1.0f};


//===============STRUCTS AND GLOBAL VARIABLES===============

//Tile struct
typedef struct Tile
{
	int tileID;
	float x; //coordinate in game space
	float y; //coordinate in game space
	float color[4];
	Tile& operator=(const Tile&);
	void printTileInfo();
} Tile_t;

//assignment operator overload for Tile struct
Tile& Tile::operator=(const Tile& tile)
{
	if(this!=&tile)
	{
		this->x = tile.x;
		this->y = tile.y;
		for(int i=0; i<4; i++)
			this->color[i] = tile.color[i];
	}
	return *this;
}

//Coordinate in the board
typedef struct BoardCoor
{
	int x;
	int y;
} BoardCoor_t;

//BoardCell struct (contents of board array)
typedef struct BoardCell
{
	int colorCode;
	int tileID; //-1 if there is no tile
} BoardCell_t;

//game board (filled with numbers 0 to 8)
BoardCell_t board[4][4];

//vector to store tileVec currently present in the game
vector<Tile_t> tileVec;

//vector containing tileIDs of removed tile
vector<int> remTilesID;

//game parameters
bool gameOver;
bool boardUpdated;
bool disableAnimation;
int score;

//==========================================================


/*****************FUNCTIONS*****************/
/*******************BELOW*******************/

//initialize board (make all location empty)
void initBoard()
{
	for(int y=0; y<4; y++)
		for(int x=0; x<4; x++)
		{
			board[y][x].colorCode = NO_TILE;
			board[y][x].tileID = -1;
		}
}

//print board (for debugging)
void printBoard()
{
	for(int y=3; y>=0; y--)
	{
		for(int x=0; x<4; x++)
			cout<<board[y][x].colorCode<<","<<board[y][x].tileID<<"  ";
		cout<<endl;
	}
	cout<<endl;
}

//print tileVec (for debugging)
void printTileVec()
{
	for(int i=0; i<tileVec.size(); i++)
	{
		cout << "(" << tileVec[i].x << "," << tileVec[i].y << "); { ";
		for(int c=0; c<4; c++)
			cout << tileVec[i].color[c] << " ";
		cout << "}\n";
	}
	cout<<endl;
}

//create 4x4 grid
void drawGrid()
{
	//horizontal lines
	drawLine(-8.0f, 8.0f, 8.0f, 8.0f, GRID_COLOR);
	drawLine(-8.0f, 4.0f, 8.0f, 4.0f, GRID_COLOR);
	drawLine(-8.0f, 0.0f, 8.0f, 0.0f, GRID_COLOR);
	drawLine(-8.0f,-4.0f, 8.0f,-4.0f, GRID_COLOR);
	drawLine(-8.0f,-8.0f, 8.0f,-8.0f, GRID_COLOR);
	//vertical lines
	drawLine(-8.0f,-8.0f,-8.0f, 8.0f, GRID_COLOR);
	drawLine(-4.0f,-8.0f,-4.0f, 8.0f, GRID_COLOR);
	drawLine( 0.0f,-8.0f, 0.0f, 8.0f, GRID_COLOR);
	drawLine( 4.0f,-8.0f, 4.0f, 8.0f, GRID_COLOR);
	drawLine( 8.0f,-8.0f, 8.0f, 8.0f, GRID_COLOR);
}

//returns true if board is full, false otherwise
bool boardFull()
{
	for(int y=0; y<4; y++)
		for(int x=0; x<4; x++)
			if(board[y][x].colorCode==NO_TILE)
				return false;
	return true;
	//return (tileVec.size()==16);
}

//creates a new tile...
void newTile(int x)
{
	cout<<"newTile() called!\n";

	gameOver = boardFull();

	if(gameOver)
		return;

	boardUpdated = false;
	disableAnimation = true;

	Tile_t newTile;

	do
	{
		newTile.x = rand()%4;
		newTile.y = rand()%4;
	} while(board[(int)newTile.y][(int)newTile.x].colorCode!=NO_TILE);

	int p = rand()%100;

	if(p<50) //50% chance red
		for(int i=0; i<4; i++)
			newTile.color[i] = RED[i];
	else //50% chance orange
		for(int i=0; i<4; i++)
			newTile.color[i] = ORANGE[i];

	newTile.tileID = tileVec.size();

	cout<<"newTile.tileID = "<<newTile.tileID<<endl<<endl;

	board[(int)newTile.y][(int)newTile.x].colorCode = p<50?1:2;
	board[(int)newTile.y][(int)newTile.x].tileID = newTile.tileID;

	tileVec.push_back(newTile);

	printBoard();
	printTileVec();
}

/*bool canMoveUp(const Tile_t& tile)
{
	if(tile.y>=2.95)
		return false;

	vector<Tile_t> tmpTileVec = tileVec;
	for(int i=0; i<tmpTileVec.size(); i++)
		if(tile.tileID == tmpTileVec[i].tileID)
		{
			tmpTileVec.erase(tmpTileVec.begin()+i);
			break;
		}

	for(int i=0; i<tmpTileVec.size(); i++)
		if(tile.x+1==tmpTileVec[i])


	return true;
}

bool allCannotMoveUp()
{
	for(int i=0; i<tileVec.size(); i++)
		if(canMoveUp(tileVec[i]))
			return false;
	return true;
}*/

BoardCoor_t getCoorByID(int tileID)
{
	for(int y=0; y<4; y++)
		for(int x=0; x<4; x++)
			if(board[y][x].tileID==tileID)
			{
				BoardCoor_t ret;
				ret.x=x;
				ret.y=y;
				return ret;
			}
}

bool allTilesInCorrectPlaces()
{
	for(int i=0; i<tileVec.size(); i++)
	{
		BoardCoor_t tmpBoardCoor = getCoorByID(tileVec[i].tileID);
		if(tmpBoardCoor.x!=tileVec[i].x || tmpBoardCoor.y!=tileVec[i].y)
			return false;
	}
	return true;
}

void moveUp()
{
	cout<<"moveUp() called!\n";

	disableAnimation = false;

	for(int i=0; i<3; i++)
		for(int y=0; y<4; y++)
			for(int x=0; x<4; x++)
				if(board[y][x].colorCode!=NO_TILE && y<3)
				{
					if(board[y+1][x].colorCode==NO_TILE || board[y+1][x].colorCode==board[y][x].colorCode)
					{
						cout<<"yay\n";
						if(board[y+1][x].colorCode==board[y][x].colorCode)
						{
							board[y+1][x].colorCode = board[y][x].colorCode+1;
							board[y+1][x].tileID = board[y][x].tileID;
						}
						else
						{
							board[y+1][x].colorCode = board[y][x].colorCode;
							board[y+1][x].tileID = board[y][x].tileID;
						}
						board[y][x].colorCode = NO_TILE;
						board[y][x].tileID = -1;
					}
				}

	cout<<endl;

	printBoard();
	printTileVec();
}

void moveUpAnimation(int value)
{
	if(!allTilesInCorrectPlaces() && !disableAnimation)
		glutTimerFunc(10, moveUpAnimation, value);

	for(int i=0; i<tileVec.size(); i++)
	{
		BoardCoor_t destCoor = getCoorByID(tileVec[i].tileID);
		if(destCoor.y<2)
		{
			if(tileVec[i].y < destCoor.y)
				tileVec[i].y+=0.05f;
		}
		else
		{
			if(tileVec[i].y < destCoor.y-0.05)
				tileVec[i].y+=0.05f;
		}
		//cout<<tileVec[i].y<<endl;
	}
}

void initGame()
{
	gameOver = false;
	boardUpdated = false;
	score = 0;
	initBoard();
	tileVec.clear();
	newTile(16.5);
}

//render a single tile on xy-position of the board ( (0,0) is left bottom )
void drawTile(float x, float y, const float color[4])
{
	drawSquare(4*x-6.0f, 4*y-6.0f, color);
}

//render current tileVec in the grid
void displaytileVec()
{
	for(int i=0; i<tileVec.size(); i++)
		drawTile(tileVec[i].x, tileVec[i].y, tileVec[i].color);
}

//display the text specified by str onto the screen at coordinate (x,y)
void displayTxt(float x, float y, const string& str)
{
	glColor4f(1.0, 0.0f, 0.0f, 1.0f);
	glRasterPos2f(x, y);
	for(int i=0; i<str.length(); i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
}

//rendering function
void display()
{
	drawClear(BACKGROUND_COLOR);

	displaytileVec();

	drawGrid();

	glutSwapBuffers();
}

//handle arrow key keypresses
void special(int key, int x, int y)
{
	if(gameOver)
		return;

	switch(key)
	{
		case GLUT_KEY_UP:
			moveUp();
			glutTimerFunc(0, moveUpAnimation, 16.5);
			/*if(!disableAnimation)
				glutTimerFunc(1000, newTile, 16.5);*/
			break;

		case GLUT_KEY_RIGHT:
			break;

		case GLUT_KEY_LEFT:
			break;

		case GLUT_KEY_DOWN:
			break;

		default:
			break;
	}
}

//handles standard keypresses
void keyboard(unsigned char key, int x, int y)
{
	switch(key) 
	{
		case 033: //Both escape key and 'q' cause the game to exit
		    exit(EXIT_SUCCESS);
		    break;
		case 'q':
			exit (EXIT_SUCCESS);
			break;
		case 'r': //'r' key restarts the game
			initGame();
			break;
		case ' ':
			break;
		case 't':
			newTile(16.5);
			break;
		default:
			break;
	}
	glutPostRedisplay();
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	srand(time(NULL));

	//configure window and display context
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(334, 34); //center game window on 1360x768 screen
	glutCreateWindow( argv[0] );
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glPointSize(4.0f); //big chunky points

	//disable key repeat
	glutIgnoreKeyRepeat(1);

	//set up a 2D top-down view
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-10.0f, 10.0f, -10.0f, 10.0f, -1.0f, 1.0f);

	initGame();

	//callback functions
	glutDisplayFunc(display);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop(); //start main loop

	return 0;
}