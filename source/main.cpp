#include "AIE.h"
#include <iostream>
#include "time.h"
#include <fstream>

using namespace std;

void UpdateGameState(float a_DeltaTime);

// My read me file from a I/O file
void readme();

enum GAMESTATES{
	MAINMENU,
	GAMEPLAY,
	HOWTOPLAY,
	HIGHSCORES,
};
enum BALLDIRECT{
	UP,
	DOWN,
};
enum ROUNDSTATE{
	SERVE,
	ROUND,
	ONEWINS,
	TWOWINS,
};



//Enum States
BALLDIRECT upDownCurrent;
BALLDIRECT GetRandY();
ROUNDSTATE gameState;


//global variables
const int screenWidth = 800;
const int screenHeight = 600;

class Paddle{
public:


	unsigned int spriteID;
	unsigned int Score;
	float pSpeed;
	float pWidth;
	float pHeight;
	void SetSize(float a_pWidth, float a_pHeight){
		pWidth = a_pWidth;
		pHeight = a_pHeight;
	}

	float x;
	float y;
	void SetPosition(float a_x, float a_y){
		x = a_x;
		y = a_y;
	}

	unsigned int moveUpKey;
	unsigned int moveDownKey;
	void SetMovementKeys(unsigned int a_moveUp, unsigned int a_moveDown){
		moveUpKey = a_moveUp;
		moveDownKey = a_moveDown;
	}

	unsigned int moveUpExt;
	unsigned int moveDownExt;
	void SetMoveExt(unsigned int a_upExt, unsigned int a_downExt){
		moveUpExt = a_upExt;
		moveDownExt = a_downExt;
	}
	void Move(float a_pTimeStep, float a_pSpeed){
		if (IsKeyDown(moveUpKey)){
			if (y < moveUpExt){
				y += pSpeed;
			}
		}

		if (IsKeyDown(moveDownKey)){
			if (y > moveDownExt + pHeight * .5f){
				y -= pSpeed;
			}
		}
		MoveSprite(spriteID, x, y);
	}
	unsigned int score = 0;
	void AddScore(unsigned int a_score)
	{
		score += a_score;
	}

	void GetScore(char* a_result)
	{
		itoa(score, a_result, 10);
	}


};
Paddle playerOne;
Paddle playerTwo;

class Ball{
public:
	unsigned int spriteID;
	float bSpeed;
	float bGravity;
	float x;
	float y;

	unsigned int bWidth;
	unsigned int bHeight;

	unsigned int moveUpExt;
	unsigned int moveDownExt;

	unsigned int moveRightExt;
	unsigned int moveLeftExt;

	bool moveLeft;
	bool moveRight;

	void SetPosition(float a_x, float a_y){
		x = a_x;
		y = a_y;
	}


	void SetSize(unsigned int a_bWidth, unsigned int a_bHieght){
		bWidth = a_bWidth;
		bHeight = a_bHieght;
	}


	void SetUpDownExt(unsigned int a_moveUpExt, unsigned int a_moveDownExt){
		moveUpExt = a_moveUpExt;
		moveDownExt = a_moveDownExt;
	}


	void SetLeftRightExt(unsigned int a_moveRightExt, unsigned int a_moveLeftExt){
		moveRightExt = a_moveRightExt;
		moveLeftExt = a_moveLeftExt;
	}



	void UpDownMove(float a_fTimeStep, float a_bGravity){

		switch (upDownCurrent){
		case UP:
			if (y >= moveUpExt){
				upDownCurrent = DOWN;
			}
			else {
				y += a_bGravity * a_fTimeStep;
			}
			break;
		case DOWN:
			if (y <= moveDownExt){
				upDownCurrent = UP;
			}

			else {
				y -= a_bGravity * a_fTimeStep;
			}
			break;
		}
	}

	void LeftRightMove(float a_fTimeStep, float a_bSpeed){

		x += bSpeed * a_fTimeStep;

	}

	bool CollideOne(){
		if (x - bWidth * .5f <= playerOne.x + playerOne.pWidth * .5f &&
			y - bHeight * .5f <= playerOne.y + playerOne.pHeight * .5f &&
			y + bHeight * .5f >= playerOne.y - playerOne.pHeight * .5f){
			while (x >= playerOne.x){
				return true;
			}
		}
		else{
			return false;
		}
	}

	bool CollideTwo(){
		if (x + bWidth * .5f >= playerTwo.x - playerTwo.pWidth * .5f &&
			y - bHeight * .5f <= playerTwo.y + playerTwo.pHeight * .5f&&
			y + bHeight * .5f >= playerTwo.y - playerTwo.pHeight * .5f){
			while (x <= playerTwo.x){
				return true;
			}
		}
		else{
			return false;
		}
	}
};
Ball GameBall;

int mHighScores;

int main(int argc, char* argv[])
{
	bool runGame = true;
	srand(time(nullptr));
	upDownCurrent = GetRandY();

	playerOne.SetSize(10, 120);
	playerOne.SetPosition(50.f, 180);
	playerOne.SetMovementKeys('W', 'S');
	playerOne.pSpeed = .2f;
	playerOne.SetMoveExt(screenHeight - playerOne.pHeight * .5f, 0);
	playerOne.Score = 0;

	playerTwo.SetSize(10, 120);
	playerTwo.SetPosition(screenWidth - 50.f, 300);
	playerTwo.SetMovementKeys('I', 'K');
	playerTwo.pSpeed = .2f;
	playerTwo.SetMoveExt(screenHeight - playerTwo.pHeight * .5f, 0);
	playerTwo.Score = 0;

	GameBall.SetPosition(screenWidth * .5f, screenHeight * .5f);
	GameBall.SetSize(18, 18);
	int randXDirection;
	int i = rand() % 2;
	if (i == 1){
		randXDirection = 1;
	}
	else {
		randXDirection = -1;
	}
	GameBall.bSpeed = 300.f * randXDirection;
	GameBall.bGravity = 100.f;
	GameBall.SetUpDownExt(screenHeight - 9.0f, 9.f);
	GameBall.SetLeftRightExt(0, screenWidth);


	Initialise(screenWidth, screenHeight, false, "Fucking Pong");
	SetBackgroundColour(SColour(0, 0, 0, 255));

	playerOne.spriteID = CreateSprite("./images/PlayerPaddle.png", playerOne.pWidth, playerOne.pHeight, true);
	playerTwo.spriteID = CreateSprite("./images/PlayerPaddle.png", playerTwo.pWidth, playerTwo.pHeight, true);
	GameBall.spriteID = CreateSprite("./images/PlayerPaddle.png", GameBall.bWidth, GameBall.bHeight, true);

	GAMESTATES currentState = MAINMENU;

	//Game Loop
	do
	{
		float deltaT = GetDeltaTime();
		switch (currentState){
		case MAINMENU:
			DrawString("Welcome To Pong", screenWidth * .5f, screenHeight *.5f);
			DrawString("Press Enter To Sart Game", screenWidth * .5f, 260);
			DrawString("If you dont know how to play...", screenWidth * .5f, 220);
			DrawString("You're dumb.", screenWidth * .5f, 180);
			DrawString("Press Q to learn how", screenWidth * .5f, 140);
			DrawString("Press Esc to Quit", screenWidth * .5f, 100);
			DrawString("Press H For HighScores", screenWidth * .5f, 60);

			if (IsKeyDown(72)){
				currentState = HIGHSCORES;
			}
			if (IsKeyDown(257)){
				currentState = GAMEPLAY;
			}

			if (IsKeyDown(81)){
				currentState = HOWTOPLAY;
			}

			if (IsKeyDown(256)){
				runGame = false;
			}
			ClearScreen();
			break;
		case GAMEPLAY:
			GameBall.bSpeed + 50.f;
			ClearScreen();

			//Drawing the Score
			char playerOneScore[3];
			char playerTwoScore[3];
			playerOne.GetScore(playerOneScore);
			playerTwo.GetScore(playerTwoScore);
			DrawString(playerOneScore, 100, 500);
			DrawString(playerTwoScore, 700, 500);
			if (playerOne.score >= 10){
				gameState = ONEWINS;
			}
			if (playerTwo.score >= 10){
				gameState = TWOWINS;

			}
			switch (gameState){
			case SERVE:
				GameBall.bSpeed *= -1.f;
				GameBall.x = screenWidth * .5f;
				GameBall.y = screenHeight * .5f;
				gameState = ROUND;
				break;
			case ROUND:
				if (GameBall.x <= 0){
					playerTwo.AddScore(1);
					gameState = SERVE;
				}
				if (GameBall.x >= screenWidth){
					playerOne.AddScore(1);
					gameState = SERVE;
				}
				break;
			case ONEWINS:
				ClearScreen();
				DrawString("Player One WINS", screenWidth * .35f, screenHeight * .5f);
				DrawString("Backspace to go to the menu", screenWidth * .3f, screenHeight * .4f);
				mHighScores = playerOne.score
					;
				if (IsKeyDown(259)){
					currentState = MAINMENU;
				}
				break;
			case TWOWINS:
				ClearScreen();
				DrawString("Player Two WINS", screenWidth * .35f, screenHeight * .5f);
				DrawString("Backspace to go to the menu", screenWidth * .3f, screenHeight * .4f);
				mHighScores = playerTwo.score;
				if (IsKeyDown(259)){
					currentState = MAINMENU;
				}
				break;
			default:
				break;
			}

			UpdateGameState(deltaT);
			DrawSprite(playerOne.spriteID);
			DrawSprite(playerTwo.spriteID);
			DrawSprite(GameBall.spriteID);


			if (IsKeyDown(256)){
				runGame = false;
			}
			break;

		case HOWTOPLAY:
			ClearScreen();
			DrawString("Hey Dumby", screenWidth * .05f, 500);
			DrawString("player one uses W and S to move his character", screenWidth * .05f, 460);
			DrawString("player Two uses I and K to move his character", screenWidth * .05f, 420);
			DrawString("Press Spacebar to go back to the main menu", screenWidth * .05f, 140);



			if (IsKeyDown(32)){
				currentState = MAINMENU;
			}
			break;
		case HIGHSCORES:
			ClearScreen();
			char buff[30];
			DrawString("the Highscore:", screenWidth * .3f, screenHeight * .5f);
			DrawString(itoa(mHighScores, buff, 10), screenWidth * .3f, screenHeight * .4f);
			DrawString("Backspace to go to the menu", screenWidth * .3f, screenHeight * .2f);
			if (IsKeyDown(259)){
				currentState = MAINMENU;
			}
		}
	} while ((!FrameworkUpdate()) && runGame == true);

	Shutdown();


	return 0;
}

void UpdateGameState(float a_DeltaTime){

	//Collision check in game
	if (GameBall.bSpeed > 0){
		if (GameBall.CollideTwo() == true){
			GameBall.bSpeed *= -1.f;
		}
	}
	if (GameBall.bSpeed < 0){
		if (GameBall.CollideOne() == true){
			GameBall.bSpeed *= -1.f;
		}
	}
	MoveSprite(GameBall.spriteID, GameBall.x, GameBall.y);
	playerOne.Move(a_DeltaTime, playerOne.pSpeed);
	playerTwo.Move(a_DeltaTime, playerOne.pSpeed);
	GameBall.LeftRightMove(a_DeltaTime, GameBall.bSpeed);
	GameBall.UpDownMove(a_DeltaTime, GameBall.bGravity);
}

BALLDIRECT GetRandY(){
	int i = rand() % 2;
	if (i == 1){
		return UP;
	}
	else{
		return DOWN;
	}
}

void WriteHighScore()
{
	fstream Highscores;
	Highscores.open("Highscores.txt", ios::out);
	Highscores << mHighScores;
	Highscores.close();
}

void LoadHighScore()
{
	fstream Highscores;
	Highscores.open("Highscores.txt", ios::in);
	char buffer[10];
	Highscores.getline(buffer, 10);
	mHighScores = atoi(buffer);
	Highscores.close();
}

