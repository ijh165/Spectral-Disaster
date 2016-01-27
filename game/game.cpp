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


//board contents definitions
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


//===============GAME OBJECTS AND PARAMETERS===============

//tile struct
typedef struct Tile
{
	int tileID;
	float x;
	float y;
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

//print tile info (for debugging)
void Tile::printTileInfo()
{
	cout << "(" << x << "," << y << "); {";
	for(int i=0; i<4; i++)
		if(i!=3)
			cout << color[i] << ",";
	cout << "}\n";
}

//game board (filled with numbers 0 to 8)
int board[4][4];

//vector to store tileVec currently present in the game
vector<Tile_t> tileVec;

//game parameters
bool gameOver;
int score;

//=========================================================


/*****************FUNCTIONS*****************/
/*******************BELOW*******************/


//initialize board (make all location empty)
void initBoard()
{
	for(int y=0; y<4; y++)
		for(int x=0; x<4; x++)
			board[y][x] = NO_TILE;
}

//print board (for debugging)
void printBoard()
{
	for(int y=3; y>=0; y--)
	{
		for(int x=0; x<4; x++)
			cout<<board[y][x]<<" ";
		cout<<endl;
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
			if(board[y][x]==NO_TILE)
				return false;
	return true;
	//return (tileVec.size()==16);
}

//returns true if board location (x,y) is occupied, false otherwise
bool isOccupied(int x, int y)
{
	return board[y][x]!=0;
}

//creates a new tile...
void newTile()
{
	gameOver = boardFull();

	if(gameOver)
		return;

	Tile_t newTile;

	do
	{
		newTile.x = rand()%4;
		newTile.y = rand()%4;
	} while(isOccupied(newTile.x, newTile.y));

	int p = rand()%100;

	if(p<50) //50% chance red
		for(int i=0; i<4; i++)
			newTile.color[i] = RED[i];
	else //50% change orange
		for(int i=0; i<4; i++)
			newTile.color[i] = ORANGE[i];

	newTile.tileID = tileVec.size();
	cout<<newTile.tileID<<endl;

	board[(int)newTile.y][(int)newTile.x] = p<50?1:2;
	tileVec.push_back(newTile);

	printBoard();
}

bool canMoveUp(const Tile_t& tile)
{
	if(tile.y>=2.95)
		return false;

	/*vector<Tile_t> tmpTileVec = tileVec;
	for(int i=0; i<tmpTileVec.size(); i++)
		if(tile.tileID == tmpTileVec[i].tileID)
		{
			tmpTileVec.erase(tmpTileVec.begin()+i);
			break;
		}

	for(int i=0; i<tmpTileVec.size(); i++)
		if(tile.x+1==tmpTileVec[i])*/


	return true;
}

bool allCannotMoveUp()
{
	for(int i=0; i<tileVec.size(); i++)
		if(canMoveUp(tileVec[i]))
			return false;
	return true;
}

void moveUp(int value)
{
	cout<<"SAVAGE\n";

	for(int i=0; i<3; i++)
		for(int y=0; y<4; y++)
			for(int x=0; x<4; x++)
				if(board[y][x]!=NO_TILE && y<3)
				{
					if(board[y+1][x]==NO_TILE || board[y+1][x]==board[y][x])
					{
						cout<<"yay\n";
						board[y+1][x] = board[y][x];
						board[y][x] = NO_TILE;
					}
				}



	printBoard();

	/*for(int i=0; i<tileVec.size(); i++)
	{
		if(canMoveUp(tileVec[i]))
			tileVec[i].y+=0.05f;
		cout<<tileVec[i].y<<endl;
	}
	if(!allCannotMoveUp())
		glutTimerFunc(5, moveUpAnimation, value);*/
}

void initGame()
{
	gameOver = false;
	score = 0;
	initBoard();
	tileVec.clear();
	newTile();
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
	switch(key)
	{
		case GLUT_KEY_UP:
			moveUp(-1);
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
			newTile();
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

	//set up a 2D top-down view
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-10.0, 10.0, -10.0, 10.0, -1.0, 1.0);

	initGame();

	//callback functions
	glutDisplayFunc(display);
	glutSpecialFunc(special);
	//glutIgnoreKeyRepeat(1); //disable key repeat
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop(); //start main loop

	return 0;
}