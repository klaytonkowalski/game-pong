//////////////////////////////////////////////////////////////////////
// LICENSE
//////////////////////////////////////////////////////////////////////

// MIT License

// Copyright (c) 2021 Klayton Kowalski

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// https://github.com/klaytonkowalski/game-pong

//////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////

#include "raylib.h"

#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// ENUMERATIONS
//////////////////////////////////////////////////////////////////////

typedef enum GameState
{
    start,
    play,
    score
}
GameState;

//////////////////////////////////////////////////////////////////////
// CONSTANTS
//////////////////////////////////////////////////////////////////////

static const int windowWidth = 960;
static const int windowHeight = 540;

static const int paddleWidth = 20;
static const int paddleHeight = 80;
static const int paddleMargin = 40;
static const int paddleSpeed = 5;

static const int ballRadius = 20;
static const int ballDiameter = ballRadius * 2;
static const int ballSpeed = 8;
static const int ballSpeedFast = 16;

static const float startThreshold = 0.5;
static const float scoreThreshold = 0.5;

static const int textSize = 20;
static const int scoreSize = 40;

//////////////////////////////////////////////////////////////////////
// PROPERTIES
//////////////////////////////////////////////////////////////////////

static Rectangle playerPaddle;
static Rectangle opponentPaddle;

static Vector2 ball;
static Vector2 ballVelocity;
static Color ballColor;

static GameState gameState;

static bool showStartText;
static float startElapsed;

static bool showScoreText;
static float scoreElapsed;

static int leftScore;
static int rightScore;
static bool leftScoredLast;

//////////////////////////////////////////////////////////////////////
// FUNCTIONS
//////////////////////////////////////////////////////////////////////

void Init()
{
    InitWindow(windowWidth, windowHeight, "Pong");
    SetTargetFPS(60);
    playerPaddle.x = paddleMargin;
    playerPaddle.y = windowHeight * 0.5 - paddleHeight * 0.5;
    playerPaddle.width = paddleWidth;
    playerPaddle.height = paddleHeight;
    opponentPaddle.x = windowWidth - paddleWidth - paddleMargin;
    opponentPaddle.y = windowHeight * 0.5 - paddleHeight * 0.5;
    opponentPaddle.width = paddleWidth;
    opponentPaddle.height = paddleHeight;
    ball.x = windowWidth * 0.5;
    ball.y = windowHeight * 0.5;
    ballVelocity.x = 0;
    ballVelocity.y = 0;
    ballColor = BLUE;
    gameState = start;
    showStartText = true;
    startElapsed = 0;
    showScoreText = true;
    scoreElapsed = 0;
    leftScore = 0;
    rightScore = 0;
    leftScoredLast = false;
}

void Update()
{
    if (gameState == start)
    {
        if (IsKeyDown(KEY_SPACE))
        {
            startElapsed = 0;
            showStartText = true;
            gameState = play;
            ballVelocity.x = GetRandomValue(0, 1) == 0 ? ballSpeed : -ballSpeed;
            ballVelocity.y = GetRandomValue(0, 1) == 0 ? ballSpeed : -ballSpeed;
        }
        else
        {
            startElapsed += GetFrameTime();
            if (startElapsed > startThreshold)
            {
                startElapsed = 0;
                showStartText = !showStartText;
            }
        }
    }
    else if (gameState == play)
    {
        if (IsKeyDown(KEY_W))
            playerPaddle.y -= paddleSpeed;
        if (IsKeyDown(KEY_S))
            playerPaddle.y += paddleSpeed;
        if (playerPaddle.y < 0)
            playerPaddle.y = 0;
        else if (playerPaddle.y > windowHeight - paddleHeight)
            playerPaddle.y = windowHeight - paddleHeight;
        if (opponentPaddle.y + paddleHeight * 0.5 < ball.y)
            opponentPaddle.y += paddleSpeed;
        else if (opponentPaddle.y + paddleHeight * 0.5 > ball.y)
            opponentPaddle.y -= paddleSpeed;
        if (opponentPaddle.y < 0)
            opponentPaddle.y = 0;
        else if (opponentPaddle.y > windowHeight - paddleHeight)
            opponentPaddle.y = windowHeight - paddleHeight;
        ball.x += ballVelocity.x;
        if (ball.x < 0)
        {
            gameState = score;
            ++rightScore;
            leftScoredLast = false;
        }
        else if (ball.x > windowWidth - ballDiameter)
        {
            gameState = score;
            ++leftScore;
            leftScoredLast = true;
            showScoreText = true;
        }
        ball.y += ballVelocity.y;
        if (ball.y < ballRadius)
        {
            ball.y = ballRadius;
            ballVelocity.y = -ballVelocity.y;
        }
        else if (ball.y > windowHeight - ballRadius)
        {
            ball.y = windowHeight - ballRadius;
            ballVelocity.y = -ballVelocity.y;
        }
        if (CheckCollisionCircleRec(ball, ballRadius, playerPaddle))
        {
            ball.x = playerPaddle.x + paddleWidth + ballRadius;
            const int randomValue = GetRandomValue(0, 2);
            if (randomValue == 0)
            {
                ballVelocity.x = ballSpeedFast;
                ballVelocity.y = (ballVelocity.y < 0 ? -1 : 1) * ballSpeedFast;
                ballColor = RED;
            }
            else
            {
                ballVelocity.x = ballSpeed;
                ballVelocity.y = (ballVelocity.y < 0 ? -1 : 1) * ballSpeed;
                ballColor = BLUE;
            }
        }
        else if (CheckCollisionCircleRec(ball, ballRadius, opponentPaddle))
        {
            ball.x = opponentPaddle.x - ballDiameter;
            const int randomValue = GetRandomValue(0, 2);
            if (randomValue == 0)
            {
                ballVelocity.x = -ballSpeedFast;
                ballVelocity.y = (ballVelocity.y < 0 ? -1 : 1) * ballSpeedFast;
                ballColor = RED;
            }
            else
            {
                ballVelocity.x = -ballSpeed;
                ballVelocity.y = (ballVelocity.y < 0 ? -1 : 1) * ballSpeed;
                ballColor = BLUE;
            }
        }
    }
    else if (gameState == score)
    {
        if (IsKeyDown(KEY_SPACE))
        {
            scoreElapsed = 0;
            showScoreText = true;
            gameState = play;
            ball.x = windowWidth * 0.5;
            ball.y = windowHeight * 0.5;
            ballVelocity.x = GetRandomValue(0, 1) == 0 ? ballSpeed : -ballSpeed;
            ballVelocity.y = GetRandomValue(0, 1) == 0 ? ballSpeed : -ballSpeed;
            ballColor = BLUE;
            playerPaddle.y = windowHeight * 0.5 - paddleHeight * 0.5;
            opponentPaddle.y = windowHeight * 0.5 - paddleHeight * 0.5;
        }
        else
        {
            scoreElapsed += GetFrameTime();
            if (scoreElapsed > scoreThreshold)
            {
                scoreElapsed = 0;
                showScoreText = !showScoreText;
            }
        }
    }
}

void Draw()
{
    BeginDrawing();
    ClearBackground(BLACK);
    DrawRectangleRec(playerPaddle, GREEN);
    DrawRectangleRec(opponentPaddle, YELLOW);
    DrawCircleV(ball, ballRadius, ballColor);
    if (gameState == start)
    {
        if (showStartText)
        {
            const int textHalfWidth = MeasureText("Press SPACE To Start!", textSize) * 0.5;
            DrawText("Press SPACE To Start!", windowWidth * 0.5 - textHalfWidth, 40, textSize, WHITE);
        }
    }
    else if (gameState == score)
    {
        if (leftScoredLast)
        {
            const int textHalfWidth = MeasureText("Player Scored!", textSize) * 0.5;
            DrawText("Player Scored!", windowWidth * 0.5 - textHalfWidth, 80, textSize, WHITE);
        }
        else
        {
            const int textHalfWidth = MeasureText("Opponent Scored!", textSize) * 0.5;
            DrawText("Opponent Scored!", windowWidth * 0.5 - textHalfWidth, 80, textSize, WHITE);
        }
        char leftScoreBuffer[8];
        sprintf(leftScoreBuffer, "%d", leftScore);
        const int leftScoreHalfWidth = MeasureText(leftScoreBuffer, scoreSize);
        DrawText(leftScoreBuffer, windowWidth * 0.25 - leftScoreHalfWidth, 50, scoreSize, GREEN);
        char rightScoreBuffer[8];
        sprintf(rightScoreBuffer, "%d", rightScore);
        const int rightScoreHalfWidth = MeasureText(rightScoreBuffer, scoreSize);
        DrawText(rightScoreBuffer, windowWidth * 0.75 - rightScoreHalfWidth, 50, scoreSize, YELLOW);
        if (showScoreText)
        {
            const int textHalfWidth = MeasureText("Press SPACE To Continue!", textSize) * 0.5;
            DrawText("Press SPACE To Continue!", windowWidth * 0.5 - textHalfWidth, 40, textSize, WHITE);
        }
    }
    EndDrawing();
}

int main(int argc, char *argv[])
{
    Init();
    while (!WindowShouldClose())
    {
        Update();
        Draw();
    }
    CloseWindow();
    return 0;
}