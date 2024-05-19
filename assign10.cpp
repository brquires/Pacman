#include <iostream>
#include<Windows.h>
//#include<gl/GL.h>

#include "smb3_font.h"

#include "Vendor/GLEW/include/GL/glew.h"
#include <cstdio>

#include "Font.hpp"

#include "GLFW/glfw3.h"
#include <cstdlib>
#include <assert.h>

#include <functional>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <thread>
#include <list>
#include <unordered_map>
#include <set>
#include <algorithm>

#include <stdlib.h>
#include <vector>
#include <deque>
#include <windows.h>
#include <iostream>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>



#define M_PI       3.14159265358979323846   // pi

#define conduct(a) a##3.4
#define conduct_(a) conduct(a)
#define strin(s)#s
#define strin_(s) strin(s)

GLFWwindow* window;

FontBitmap font("smb3_font.png", 8, 8);

// Author: Patricia Terol
// Course: CSE 2050
// Project: assign10

void Characters()
{
	if(font.getcharData()['A' - ' '].x == -1)
	{
		font.AddCharacters('A', 0, 25, 16);
		font.AddCharacters('Q', 0, 33, 10);
		font.AddCharacters('a', 0, 25, 16);
		font.AddCharacters('q', 0, 33, 10);
		font.AddCharacters('0', 0, 41, 10);
		font.AddCharacter('=', 80, 41);

		font.AddCharacter('$', 128, 16);
		font.AddCharacter('.', 136, 16);
		font.AddCharacter('!', 144, 16);
		font.AddCharacter('?', 152, 16);
	}
}

static void drawText(const std::string& text, int charWidth, int charHeight, const unsigned char* data)
{
	glDisable(GL_TEXTURE_2D);
	int x = 0;
	int y = 0;
	glBegin(GL_QUADS);
	for (std::size_t i = 0; i < text.size(); ++i)
	{
		unsigned char ch = text[i];
		if (ch > 0x7f)
		{
			ch = 0x7f;
		}

		if (ch == '\n') // newline
		{
			y++;
			x = 0;
			continue;
		}

		// Draw the right character
		for (int cx = 0; cx < charWidth; ++cx)
		{
			for (int cy = 0; cy < charHeight; ++cy)
			{
				unsigned char p = data[ch * charWidth * charHeight + (charHeight - 1 - cy) * charWidth + cx];
				if (p != 0)
				{
					if (p == 1)
					{
						glColor4ub(0xff, 0xff, 0xff, 0xff);
					}
					else
					{
						glColor4ub(0, 0, 0, 0xff);
					}
					glVertex2i(x * (charWidth * 3 + 1) + (cx * 3), y * -1 * (charHeight * 3 + 1) + (-cy * 3));
					glVertex2i(x * (charWidth * 3 + 1) + (cx * 3 + 3), y * -1 * (charHeight * 3 + 1) + (-cy * 3));
					glVertex2i(x * (charWidth * 3 + 1) + (cx * 3 + 3), y * -1 * (charHeight * 3 + 1) + (-cy * 3 + 3));
					glVertex2i(x * (charWidth * 3 + 1) + (cx * 3), y * -1 * (charHeight * 3 + 1) + (-cy * 3 + 3));
				}
			}
		}
		x++;
	}
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void drawText(const std::string& text)
{
	drawText(text, FONT_WIDTH, FONT_HEIGHT, &fontData[0][0][0]);
}


static bool replay = false; //check if starts a new game
static bool over = true; //check for the game to be over
static float squareSize = 50.0; //size of one square on the game
static float xIncrement = 0; // x movement on pacman
static float yIncrement = 0; // y movement on pacman
static int rotation = 0; // orientation of pacman
float* monster1 = new float[3] {10.5, 8.5, 1.0}; //coordinates and direction of first monster
float* monster2 = new float[3] {13.5, 1.5, 2.0}; //coordinates and direction of second monster
float* monster3 = new float[3] {4.5, 6.5, 3.0}; //coordinates and direction of third monster
float* monster4 = new float[3] {2.5, 13.5, 4.0}; //coordinates and direction of fourth monster
static std::vector<int> border = { 0, 0, 15, 1, 15, 15, 14, 1, 0, 14, 15, 15, 1, 14, 0, 0 }; //coordinates of the border walls

//coordinates of the obstacles (divided into 3 for clarity)
static std::vector<int> obstaclesTop = { 2, 2, 3, 6, 3, 6, 4, 5, 4, 2, 5, 4, 5, 3, 6, 5, 6, 1, 9, 2, 7, 2, 8, 5, 9, 5, 10, 3, 10, 4, 11, 2, 11, 5, 12, 6, 12, 6, 13, 2 };
static std::vector<int> obstaclesMiddle = { 2, 9, 3, 7, 3, 7, 4, 8, 4, 9, 5, 11, 5, 6, 6, 10, 6, 10, 7, 8, 7, 8, 8, 9, 6, 7, 7, 6, 8, 6, 9, 7, 10, 6, 9, 10, 9, 10, 8, 8, 11, 9, 10, 11, 11, 8, 12, 7, 12, 7, 13, 9 };
static std::vector<int> obstaclesBottom = { 2, 10, 3, 13, 3, 13, 4, 12, 5, 12, 6, 13, 6, 13, 7, 11, 8, 11, 9, 13, 9, 13, 10, 12, 11, 12, 12, 13, 12, 13, 13, 10 };
static std::deque<float> food = { 1.5, 1.5, 1.5, 2.5, 1.5, 3.5, 1.5, 4.5, 1.5, 5.5, 1.5, 6.5, 1.5, 7.5, 1.5, 8.5, 1.5, 9.5, 1.5, 10.5, 1.5, 11.5, 1.5, 12.5, 1.5, 13.5, 2.5, 1.5, 2.5, 6.5, 2.5, 9.5, 2.5, 13.5, 3.5, 1.5, 3.5, 2.5, 3.5, 3.5, 3.5, 4.5, 3.5, 6.5, 3.5, 8.5, 3.5, 9.5, 3.5, 10.5, 3.5, 11.5, 3.5, 13.5, 4.5, 1.5, 4.5, 4.5, 4.5, 5.5, 4.5, 6.5, 4.5, 7.5, 4.5, 8.5, 4.5, 11.5, 4.5, 12.5, 4.5, 13.5, 5.5, 1.5, 5.5, 2.5, 5.5, 5.5, 5.5, 10.5, 5.5, 11.5, 5.5, 13.5, 6.5, 2.5, 6.5, 3.5, 6.5, 4.5, 6.5, 5.5, 6.5, 7.5, 6.5, 10.5, 6.5, 13.5, 7.5, 5.5, 7.5, 6.5, 7.5, 7.5, 7.5, 9.5, 7.5, 10.5, 7.5, 11.5, 7.5, 12.5, 7.5, 13.5, 8.5, 2.5, 8.5, 3.5, 8.5, 4.5, 8.5, 5.5, 8.5, 7.5, 8.5, 10.5, 8.5, 13.5, 9.5, 1.5, 9.5, 2.5, 9.5, 5.5, 9.5, 10.5, 9.5, 11.5, 9.5, 13.5, 10.5, 1.5, 10.5, 4.5, 10.5, 5.5, 10.5, 6.5, 10.5, 7.5, 10.5, 8.5, 10.5, 11.5, 10.5, 12.5, 10.5, 13.5, 11.5, 1.5, 11.5, 2.5, 11.5, 3.5, 11.5, 4.5, 11.5, 5.5, 11.5, 6.5, 11.5, 8.5, 11.5, 9.5, 11.5, 10.5, 11.5, 11.5, 11.5, 13.5, 12.5, 1.5, 12.5, 6.5, 12.5, 9.5, 12.5, 13.5, 13.5, 1.5, 13.5, 2.5, 13.5, 3.5, 13.5, 4.5, 13.5, 5.5, 13.5, 6.5, 13.5, 7.5, 13.5, 8.5, 13.5, 9.5, 13.5, 10.5, 13.5, 11.5, 13.5, 12.5, 13.5, 13.5 };
static std::vector<std::vector<bool>> bitmap; // 2d image of which squares are blocked and which are clear for pacman to move in 
bool* keyStates = new bool[256]; // record of all keys pressed 
int points = 0; // total points collected

//Initializes the game with the appropiate information 
void init(void) {
	//clear screen
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	//reset all keys
	for (int i = 0; i < 256; i++) {
		keyStates[i] = false;
	}
	//fill the bitmap with the obstacles
	bitmap.push_back({ true, true, true, true, true, true, true, true, true, true, true, true, true, true, true });
	bitmap.push_back({ true, false, false, false, false, false, false, false, false, false, false, false, false, false, true });
	bitmap.push_back({ true, false, true, true, true, true, false, true, true, false, true, true, true, false, true });
	bitmap.push_back({ true, false, false, false, false, true, false, true, false, false, false, false, true, false, true });
	bitmap.push_back({ true, false, true, true, false, false, false, false, false, true, true, false, false, false, true });
	bitmap.push_back({ true, false, false, true, true, false, true, true, true, true, false, false, true, false, true });
	bitmap.push_back({ true, true, false, false, false, false, true, false, true, true, false, true, true, false, true });
	bitmap.push_back({ true, true, true, true, true, false, false, false, true, false, false, false, false, false, true });
	bitmap.push_back({ true, true, false, false, false, false, true, false, true, true, false, true, true, false, true });
	bitmap.push_back({ true, false, false, true, true, false, true, true, true, true, false, false, true, false, true });
	bitmap.push_back({ true, false, true, true, false, false, false, false, false, true, true, false, false, false, true });
	bitmap.push_back({ true, false, false, false, false, true, false, true, false, false, false, false, true, false, true });
	bitmap.push_back({ true, false, true, true, true, true, false, true, true, false, true, true, true, false, true });
	bitmap.push_back({ true, false, false, false, false, false, false, false, false, false, false, false, false, false, true });
	bitmap.push_back({ true, true, true, true, true, true, true, true, true, true, true, true, true, true, true });
}

//Method to draw the obstacle course and the walls
void drawLaberynth() {
	glColor3f(1.0, 1.0, 1.0);
	//Border
	for (int i = 0; i < border.size(); i = i + 4) {
		glRectf(border.at(i) * squareSize, border.at(i + 1) * squareSize, border.at(i + 2) * squareSize, border.at(i + 3) * squareSize);
	}

	//Obstacles
	for (int j = 0; j < obstaclesBottom.size(); j = j + 4) {
		glRectf(obstaclesBottom.at(j) * squareSize, obstaclesBottom.at(j + 1) * squareSize, obstaclesBottom.at(j + 2) * squareSize, obstaclesBottom.at(j + 3) * squareSize);
	}
	for (int k = 0; k < obstaclesMiddle.size(); k = k + 4) {
		glRectf(obstaclesMiddle.at(k) * squareSize, obstaclesMiddle.at(k + 1) * squareSize, obstaclesMiddle.at(k + 2) * squareSize, obstaclesMiddle.at(k + 3) * squareSize);
	}
	for (int p = 0; p < obstaclesTop.size(); p = p + 4) {
		glRectf(obstaclesTop.at(p) * squareSize, obstaclesTop.at(p + 1) * squareSize, obstaclesTop.at(p + 2) * squareSize, obstaclesTop.at(p + 3) * squareSize);
	}
}

//Method to check if the food has been eaten
bool foodEaten(int x, int y, float pacmanX, float pacmanY) {
	if (x >= pacmanX - 16.0 * cos(359 * M_PI / 180.0) && x <= pacmanX + 16.0 * cos(359 * M_PI / 180.0)) {
		if (y >= pacmanY - 16.0 * cos(359 * M_PI / 180.0) && y <= pacmanY + 16.0 * cos(359 * M_PI / 180.0)) {
			return true;
		}
	}
	return false;
}

//Method to draw all the food left and delete the ate one
void drawFood(float pacmanX, float pacmanY) {
	std::deque<float> temp;
	//check if the food has not been eaten
	for (int i = 0; i < food.size(); i = i + 2) {
		if (!foodEaten(food.at(i) * squareSize, food.at(i + 1) * squareSize, pacmanX, pacmanY)) {
			temp.push_back(food.at(i));
			temp.push_back(food.at(i + 1));
		}
		else {
			points++;
		}
	}
	food.swap(temp);
	glPointSize(5.0);
	glBegin(GL_POINTS);
	glColor3f(1.0, 1.0, 1.0);
	//draw all the food avilable
	for (int j = 0; j < food.size(); j = j + 2) {
		glVertex2f(food.at(j) * squareSize, food.at(j + 1) * squareSize);
	}
	glEnd();
}

//Method to draw the pacman character through consicutive circle algorithm
void drawPacman(float positionX, float positionY, float rotation) {
	int x, y;
	glBegin(GL_LINES);
	glColor3f(1.0, 1.0, 0.0);
	for (int k = 0; k < 32; k++) {
		x = (float)k / 2.0 * cos((30 + 90 * rotation) * M_PI / 180.0) + (positionX * squareSize);
		y = (float)k / 2.0 * sin((30 + 90 * rotation) * M_PI / 180.0) + (positionY * squareSize);
		for (int i = 30; i < 330; i++) {
			glVertex2f(x, y);
			x = (float)k / 2.0 * cos((i + 90 * rotation) * M_PI / 180.0) + (positionX * squareSize);
			y = (float)k / 2.0 * sin((i + 90 * rotation) * M_PI / 180.0) + (positionY * squareSize);
			glVertex2f(x, y);
		}
	}
	glEnd();
}

//Method to draw the monster character through consecutive circles algorithm
void drawMonster(float positionX, float positionY, float r, float g, float b) {
	int x, y;
	glBegin(GL_LINES);
	glColor3f(r, g, b);
	//draw the head
	for (int k = 0; k < 32; k++) {
		x = (float)k / 2.0 * cos(360 * M_PI / 180.0) + (positionX * squareSize);
		y = (float)k / 2.0 * sin(360 * M_PI / 180.0) + (positionY * squareSize);
		for (int i = 180; i <= 360; i++) {
			glVertex2f(x, y);
			x = (float)k / 2.0 * cos(i * M_PI / 180.0) + (positionX * squareSize);
			y = (float)k / 2.0 * sin(i * M_PI / 180.0) + (positionY * squareSize);
			glVertex2f(x, y);
		}
	}
	glEnd();
	//draw body
	glRectf((positionX * squareSize) - 17, positionY * squareSize, (positionX * squareSize) + 15, (positionY * squareSize) + 15);
	glBegin(GL_POINTS);
	glColor3f(0, 0.2, 0.4);
	//draw eyes and legs
	glVertex2f((positionX * squareSize) - 11, (positionY * squareSize) + 14); //legs
	glVertex2f((positionX * squareSize) - 1, (positionY * squareSize) + 14); //legs
	glVertex2f((positionX * squareSize) + 8, (positionY * squareSize) + 14); //legs
	glVertex2f((positionX * squareSize) + 4, (positionY * squareSize) - 3); //eyes
	glVertex2f((positionX * squareSize) - 7, (positionY * squareSize) - 3); //eyes 
	glEnd();
}

//Method to update the position of the monsters randomly
void updateMonster(float* monster, int id) {
	//find the current position of the monster
	int x1Quadrant = (int)((monster[0] - (2 / squareSize)) - (16.0 * cos(360 * M_PI / 180.0)) / squareSize);
	int x2Quadrant = (int)((monster[0] + (2 / squareSize)) + (16.0 * cos(360 * M_PI / 180.0)) / squareSize);
	int y1Quadrant = (int)((monster[1] - (2 / squareSize)) - (16.0 * cos(360 * M_PI / 180.0)) / squareSize);
	int y2Quadrant = (int)((monster[1] + (2 / squareSize)) + (16.0 * cos(360 * M_PI / 180.0)) / squareSize);
	//move him acording to its direction until he hits an obstacle
	switch ((int)monster[2]) {
	case 1:
		if (!bitmap.at(x1Quadrant).at((int)monster[1])) {
			monster[0] -= 2 / squareSize;
		}
		else {
			int current = monster[2];
			do {
				monster[2] = (rand() % 4) + 1;
			} while (current == (int)monster[2]);
		}
		break;
	case 2:
		if (!bitmap.at(x2Quadrant).at((int)monster[1])) {
			monster[0] += 2 / squareSize;
		}
		else {
			int current = monster[2];
			do {
				monster[2] = (rand() % 4) + 1;
			} while (current == (int)monster[2]);
		}
		break;
	case 3:
		if (!bitmap.at((int)monster[0]).at(y1Quadrant)) {
			monster[1] -= 2 / squareSize;
		}
		else {
			int current = monster[2];
			do {
				monster[2] = (rand() % 4) + 1;
			} while (current == (int)monster[2]);
		}
		break;
	case 4:
		if (!bitmap.at((int)monster[0]).at(y2Quadrant)) {
			monster[1] += 2 / squareSize;
		}
		else {
			int current = monster[2];
			do {
				monster[2] = (rand() % 4) + 1;
			} while (current == (int)monster[2]);
		}
		break;
	default:
		break;
	}
}
unsigned char Letter = 0;
//Method to set the pressed key
void keyPressed(GLFWwindow* window, unsigned int key)
{
	if (key != Letter)
	{
		Letter = key;
		for (int i = 0; i < 256; i++)
		{
			keyStates[i] = false;
		}
	keyStates[key] = true;
	}

	//Single tap Movement

	//Letter = key;
	//for (int i = 0; i < 256; i++)
	//{
	//	keyStates[i] = false;
	//}
	//keyStates[key] = true;

}

//Method to unset the released key

void keyUp(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, key) == GLFW_RELEASE)
	{
		if (key == Letter || key == (Letter - 32))
			keyStates[Letter] = false;

	}
}

//Method to reset all the variable necessaries to start the game again
void resetGame() {

	delete[] monster1; // new added
	delete[] monster2;
	delete[] monster3;
	delete[] monster4;

	over = false;
	xIncrement = 0;
	yIncrement = 0;
	rotation = 0;
	monster1 = new float[3] {10.5, 8.5, 1.0};
	monster2 = new float[3] {13.5, 1.5, 2.0};
	monster3 = new float[3] {4.5, 6.5, 3.0};
	monster4 = new float[3] {2.5, 13.5, 4.0};
	points = 0;
	for (int i = 0; i < 256; i++) {
		keyStates[i] = false;
	}
	food = { 1.5, 1.5, 1.5, 2.5, 1.5, 3.5, 1.5, 4.5, 1.5, 5.5, 1.5, 6.5, 1.5, 7.5, 1.5, 8.5, 1.5, 9.5, 1.5, 10.5, 1.5, 11.5, 1.5, 12.5, 1.5, 13.5, 2.5, 1.5, 2.5, 6.5, 2.5, 9.5, 2.5, 13.5, 3.5, 1.5, 3.5, 2.5, 3.5, 3.5, 3.5, 4.5, 3.5, 6.5, 3.5, 8.5, 3.5, 9.5, 3.5, 10.5, 3.5, 11.5, 3.5, 13.5, 4.5, 1.5, 4.5, 4.5, 4.5, 5.5, 4.5, 6.5, 4.5, 7.5, 4.5, 8.5, 4.5, 11.5, 4.5, 12.5, 4.5, 13.5, 5.5, 1.5, 5.5, 2.5, 5.5, 5.5, 5.5, 10.5, 5.5, 11.5, 5.5, 13.5, 6.5, 2.5, 6.5, 3.5, 6.5, 4.5, 6.5, 5.5, 6.5, 7.5, 6.5, 10.5, 6.5, 13.5, 7.5, 5.5, 7.5, 6.5, 7.5, 7.5, 7.5, 9.5, 7.5, 10.5, 7.5, 11.5, 7.5, 12.5, 7.5, 13.5, 8.5, 2.5, 8.5, 3.5, 8.5, 4.5, 8.5, 5.5, 8.5, 7.5, 8.5, 10.5, 8.5, 13.5, 9.5, 1.5, 9.5, 2.5, 9.5, 5.5, 9.5, 10.5, 9.5, 11.5, 9.5, 13.5, 10.5, 1.5, 10.5, 4.5, 10.5, 5.5, 10.5, 6.5, 10.5, 7.5, 10.5, 8.5, 10.5, 11.5, 10.5, 12.5, 10.5, 13.5, 11.5, 1.5, 11.5, 2.5, 11.5, 3.5, 11.5, 4.5, 11.5, 5.5, 11.5, 6.5, 11.5, 8.5, 11.5, 9.5, 11.5, 10.5, 11.5, 11.5, 11.5, 13.5, 12.5, 1.5, 12.5, 6.5, 12.5, 9.5, 12.5, 13.5, 13.5, 1.5, 13.5, 2.5, 13.5, 3.5, 13.5, 4.5, 13.5, 5.5, 13.5, 6.5, 13.5, 7.5, 13.5, 8.5, 13.5, 9.5, 13.5, 10.5, 13.5, 11.5, 13.5, 12.5, 13.5, 13.5 };
}

//Method to update the movement of the pacman according to the movement keys pressed
void keyOperations() {
	//get current position
	float  x = (1.5 + xIncrement) * squareSize;
	float y = (1.5 + yIncrement) * squareSize;
	//update according to keys pressed
	if (keyStates['a']) {
		x -= 9;
		int x1Quadrant = (int)((x - 16.0 * cos(360 * M_PI / 180.0)) / squareSize);
		if (!bitmap.at(x1Quadrant).at((int)y / squareSize)) 
		{
			int y1 = y;
			y1 -= 5;
			int y1Quadrant = (int)((y1 - 16.0 * cos(360 * M_PI / 180.0)) / squareSize);

			if (!bitmap.at(x1Quadrant).at(y1Quadrant))
			{
				int y2 = y;
				y2 += 5;
				int y2Quadrant = (int)((y2 + 16.0 * cos(360 * M_PI / 180.0)) / squareSize);
				if (!bitmap.at(x1Quadrant).at(y2Quadrant))
				{
					xIncrement -= 2 / squareSize;
					rotation = 2;
				}

			}
		}
	}
	if (keyStates['d']) {
		x += 9;
		int x2Quadrant = (int)((x + 16.0 * cos(360 * M_PI / 180.0)) / squareSize);
		if (!bitmap.at(x2Quadrant).at((int)y / squareSize)) 
		{
			int y1 = y;
			y1 -= 5;
			int y1Quadrant = (int)((y1 - 16.0 * cos(360 * M_PI / 180.0)) / squareSize);

			if (!bitmap.at(x2Quadrant).at(y1Quadrant))
			{
				int y2 = y;
				y2 += 5;
				int y2Quadrant = (int)((y2 + 16.0 * cos(360 * M_PI / 180.0)) / squareSize);
				if (!bitmap.at(x2Quadrant).at(y2Quadrant)) 
				{
					xIncrement += 2 / squareSize;
					rotation = 0;
				}

			}
		}
	}
	if (keyStates['w']) {
		y -= 9;
		int y1Quadrant = (int)((y - 16.0 * cos(360 * M_PI / 180.0)) / squareSize);
		if (!bitmap.at((int)x / squareSize).at(y1Quadrant)) 
		{
			int x1 = x;
			x1 -= 5;
			int x1Quadrant = (int)((x1 - 16.0 * cos(360 * M_PI / 180.0)) / squareSize);

			if (!bitmap.at(x1Quadrant).at(y1Quadrant))
			{
				int x2 = x;
				x2 += 5;
				int x2Quadrant = (int)((x2 + 16.0 * cos(360 * M_PI / 180.0)) / squareSize);
				if (!bitmap.at(x2Quadrant).at(y1Quadrant))
				{
					yIncrement -= 2 / squareSize;
					rotation = 3;
				}

			}
		}
	}
	if (keyStates['s'])
	{
		y += 9;
		int y2Quadrant = (int)((y + 16.0 * cos(360 * M_PI / 180.0)) / squareSize);
		if (!bitmap.at((int)x / squareSize).at(y2Quadrant))
		{
			int x1 = x;
			x1 -= 5;
			int x1Quadrant = (int)((x1 - 16.0 * cos(360 * M_PI / 180.0)) / squareSize);

			if (!bitmap.at(x1Quadrant).at(y2Quadrant))
			{
				int x2 = x;
				x2 += 5;
				int x2Quadrant = (int)((x2 + 16.0 * cos(360 * M_PI / 180.0)) / squareSize);
				if (!bitmap.at(x2Quadrant).at(y2Quadrant))
				{
					yIncrement += 2 / squareSize;
					rotation = 1;
				}

			}
		}
	}
	if (keyStates[' ']) {
		if (!replay && over) {
			resetGame();
			replay = true;
		}
		else if (replay && over) {
			replay = false;
		}
	}
}

//Method to check if the game is over
void gameOver() {
	int pacmanX = (int)(1.5 + xIncrement);
	int pacmanY = (int)(1.5 + yIncrement);
	int monster1X = (int)(monster1[0]);
	int monster1Y = (int)(monster1[1]);
	int monster2X = (int)(monster2[0]);
	int monster2Y = (int)(monster2[1]);
	int monster3X = (int)(monster3[0]);
	int monster3Y = (int)(monster3[1]);
	int monster4X = (int)(monster4[0]);
	int monster4Y = (int)(monster4[1]);
	if (pacmanX == monster1X && pacmanY == monster1Y){
		over = true;
	}
	if (pacmanX == monster2X && pacmanY == monster2Y){
		over = true;
	}
	if (pacmanX == monster3X && pacmanY == monster3Y){
		over = true;
	}
	if (pacmanX == monster4X && pacmanY == monster4Y){
		over = true;
	}
	if (points == 106){
		over = true;
	}
}



//Method to display the Result of the Game at the End
void resultsDisplay()
{
	if (points == 106) {
		//Won
		
		//glRasterPos2f(170, 250);
		glTranslatef(80, 200, 0);
		drawText("*************************************");

		glColor3f(1, 1, 1);
		glLoadIdentity();
		//glRasterPos2f(200, 300);
		glTranslatef(175, 250, 0);
		drawText("CONGRATULATIONS, YOU WON! ");

		glLoadIdentity();
		//glRasterPos2f(170, 350);
		glTranslatef(80, 300, 0);
		drawText("*************************************");

		glLoadIdentity();
		//glRasterPos2f(170, 550);
		glTranslatef(80, 400, 0);
		drawText("To start or restart the game");

		glLoadIdentity();

		glTranslatef(80, 450, 0);

		drawText("press the space key.");

		glLoadIdentity();
	}
	else {
		//Lost
		const char* message;
		//glRasterPos2f(210, 250);
		glTranslatef(170, 200, 0);
		drawText("*************************");

		glColor3f(1, 1, 1);
		glLoadIdentity();
		//glRasterPos2f(250, 300);
		glTranslatef(240, 250, 0);
		drawText("SORRY, YOU LOST ... ");

		glLoadIdentity();
		//glRasterPos2f(210, 350);
		glTranslatef(170, 300, 0);
		drawText("*************************");

		glLoadIdentity();
		//glRasterPos2f(260, 400);
		glTranslatef(200, 400, 0);
		drawText("You got: ");

		std::string result = std::to_string(points);
		message = (char*)result.c_str();

		glLoadIdentity();
		//glRasterPos2f(350, 400);
		glTranslatef(360, 400, 0);
		drawText(message);

		glLoadIdentity();
		//glRasterPos2f(385, 400);
		glTranslatef(420, 400, 0);
		drawText(" points!");

		glLoadIdentity();
		//glRasterPos2f(170, 550);
		glTranslatef(55, 500, 0);

		drawText("To restart the game, press the space key.");

		glLoadIdentity();
	}

}


//Method to Display the starting instructions of a Game //drawText function one
/*
void welcomeScreen()
{
	
	glClearColor(0, 0.2, 0.4, 1.0);

	glTranslatef(80, 200, 0);
	drawText("*************************************");

	glLoadIdentity();
	glColor3f(1, 1, 1);

	glTranslatef(175, 250, 0);
	drawText("PACMAN - by Patricia Terol");

	glLoadIdentity();

	glTranslatef(80, 300, 0);
	drawText("*************************************");

	glLoadIdentity();

	glTranslatef(50, 400, 0);
	drawText("To control Pacman");

	glLoadIdentity();

	glTranslatef(50, 430, 0);
	drawText("A to go right");
	glLoadIdentity();

	glTranslatef(50, 460, 0);
	drawText("D to go left");
	glLoadIdentity();

	glTranslatef(50, 490, 0);
	drawText("W to go up");
	glLoadIdentity();

	glTranslatef(50, 520, 0);
	drawText("S to go down.");

	glLoadIdentity();

	glTranslatef(50, 580, 0);
	drawText("To start or restart the game");

	glLoadIdentity();

	glTranslatef(50, 610, 0);
	drawText("Press the space key.");

	glLoadIdentity();
	

}
*/

//renderText function one

void welcomeScreen()
{
	static FontBitmap font1("smb3_font.png", 8, 8);
	font1.Characters(font1);

	Characters();

	glClearColor(0, 0.2, 0.4, 1.0);

	glTranslatef(80, 200, 0);
	drawText("*************************************");

	glLoadIdentity();
	glColor3f(1, 1, 1);

	glTranslatef(115, 234, 0);
	font.renderText("PACMAN by Patricia Terol", 3);

	glLoadIdentity();

	glTranslatef(80, 300, 0);
	drawText("*************************************");

	glLoadIdentity();

	glTranslatef(50, 400, 0);
	font.renderText("To control Pacman", 3);

	glLoadIdentity();

	glTranslatef(50, 430, 0);
	font.renderText("A to go right", 3);
	glLoadIdentity();

	glTranslatef(50, 460, 0);
	font.renderText("D to go left", 3);
	glLoadIdentity();

	glTranslatef(50, 490, 0);
	font.renderText("W to go up", 3);
	glLoadIdentity();

	glTranslatef(50, 520, 0);
	font.renderText("S to go down.", 3);

	glLoadIdentity();

	glTranslatef(00, 580, 0);
	font1.renderTexture("To start or restart the game", 25);

	glLoadIdentity();

	glTranslatef(105, 610, 0);
	font1.renderTexture("Press the space key.", 25);

	glLoadIdentity();
}


void welcomeScreen(float height, float width, float bytex, float bytey)
{

	glTranslatef(100, 100, 0);
	glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, font.m_id);

	float Hhalf = height / 2.0;
	float Whalf = width / 2.0;

	float HTexSize = bytex / height;
	float WTexSize = bytey / width;

	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex2d(0.0, 0.0);
	glTexCoord2d(WTexSize, 0);
	glVertex2d(100, 0.0);
	glTexCoord2d(WTexSize, HTexSize);
	glVertex2d(100, 100);
	glTexCoord2d(0, HTexSize);
	glVertex2d(0.0, 100);

	glEnd();
}

//Method to display the screen and its elements
void display() 
{


	if (points == 1) {
		over = false;
	}
	keyOperations();
	glClear(GL_COLOR_BUFFER_BIT);
	gameOver();
	if (replay) {

		if (!over) {

			glDisable(GL_BLEND);
			drawLaberynth();
			drawFood((1.5 + xIncrement) * squareSize, (1.5 + yIncrement) * squareSize);
			drawPacman(1.5 + xIncrement, 1.5 + yIncrement, rotation);
			updateMonster(monster1, 1);
			updateMonster(monster2, 2);
			updateMonster(monster3, 3);
			updateMonster(monster4, 4);
			drawMonster(monster1[0], monster1[1], 0.0, 1.0, 1.0); //cyan
			drawMonster(monster2[0], monster2[1], 1.0, 0.0, 0.0); //red
			drawMonster(monster3[0], monster3[1], 1.0, 0.0, 0.6); //magenta
			drawMonster(monster4[0], monster4[1], 1.0, 0.3, 0.0); //orange
		}
		else {
			resultsDisplay();
		}
	}
	else {
		welcomeScreen();
	}
	glfwSwapBuffers(window);
}

//Methdo to reshape the game is the screen size changes
void reshape(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glOrtho(0, 750, 750, 0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


int main(int aiy = 0, char** aie = nullptr, char* yyyy = nullptr)
{


	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */


	window = glfwCreateWindow(750, 750, "Window", NULL, 0);
	{

		if (!window)
		{
			glfwTerminate();
			return -1;
		}

		float rotation = 0.0f;

		/* Make the window's context current */
		glfwMakeContextCurrent(window);


		glfwSwapInterval(1);

		init();

		while (!glfwWindowShouldClose(window))
		{
			//FontBitmap font("smb3_font.png", 8, 8);
			//font.Characters(font);
			reshape(750, 750);
			display();

			//glClear(GL_COLOR_BUFFER_BIT);
			//glClearColor(0.4f, 0.0f, 0.0f, 1.0f);
			//font.renderTexture("AMAZON", 25);
			//welcomeScreen(font.m_Height, font.m_Width, 8, 8);
			//glfwSwapBuffers(window);
			//display();

			//Single tap Movement
			//glfwSetKeyCallback(window, (GLFWkeyfun)keyUp);

			glfwSetCharCallback(window, (GLFWcharfun)keyPressed);

			//glutKeyboardUpFunc(keyUp);

			/* Swap front and back buffers */
			//glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();

		}

	}

	glfwTerminate();

}
