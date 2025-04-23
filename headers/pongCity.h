#ifndef PONGCITY_H
#define PONGCITY_H

#include "C:\raylib\raylib\src\raylib.h"
#include "settings.h"

int aiScore = 0, playerScore = 0;
int gameLevel = 1; // 1 = Karachi, 2 = New York, 3 = Rome
float ballSpeed;
float aiSpeed;
float playerSpeed;

// Colours
Color ballColor;
Color paddleColor;
Color cBrown = {109, 65, 42, 255};
Color cGreen = {81, 116, 95, 255};
Color karachiBall = {60, 24, 24, 255};
Color newYorkBall = {106, 101, 0, 255};
Color romeBall = WHITE;
Color karachiPaddle = {255, 174, 11, 255};
Color newYorkPaddle = {81, 116, 95, 255};
Color romePaddle = {199, 60, 0, 255};

// BALL CLASS

class Ball {
    private:
        float x;
        float y;
        float radius;
        float speedX;
        float speedY;

    public:
        Ball (float x, float y, float radius, float speedX, float speedY) :
            x(x),
            y(y),
            radius(radius),
            speedX(speedX),
            speedY(speedY) {}
        
        void draw () {
            if (gameLevel == 1) {
                ballColor = karachiBall;
            } else if (gameLevel == 2) {
                ballColor = newYorkBall;
            } else if (gameLevel == 3) {
                ballColor = romeBall;
            }
            DrawCircle(x, y, radius, ballColor);
        }

        void update () {
            x += speedX;
            y += speedY;

            if ((y + radius) >= GetScreenHeight() || (y - radius) <= 0) {
                speedY *= -1;
            }

            if ((x + radius) >= GetScreenWidth()) {
                aiScore ++;
                resetBall();
            }
            
            if ((x - radius) <= 0) {
                playerScore ++;
                resetBall();
            }
        }

        void resetBall () {
            x = GetScreenWidth() / 2;
            y = GetScreenHeight() / 2;

            int speeds[2] = {1, -1};
            speedX *= speeds[GetRandomValue(0, 1)];
            speedY *= speeds[GetRandomValue(0, 1)];
        }

        float getY () {
            return y;
        }

        float getX () {
            return x;
        }

        float getRadius () {
            return radius;
        }

        void InvertSpeedX () {
            speedX *= -1;
        }

        void setSpeedX (float s) {
            speedX = s;
        }

        void setSpeedY (float s) {
            speedY = s;
        }
};


// PADDLE BASE CLASS

class Paddle {
    protected: 
        float x;
        float y;
        float width;
        float height;
        float speed;
    
    public:
        Paddle (float x, float y, float w, float h, float s) :
            x(x),
            y(y),
            width(w),
            height(h),
            speed(s) {}

        void draw () {
            if (gameLevel == 1) {
                paddleColor = karachiPaddle;
            } else if (gameLevel == 2) {
                paddleColor = newYorkPaddle;
            } else if (gameLevel == 3) {
                paddleColor = romePaddle;
            }
            
            float borderThickness = 3.0f;
            DrawRectangleRounded(Rectangle{x - borderThickness, y - borderThickness, width + 2 * borderThickness, height + 2 * borderThickness}, 0.8, 0, WHITE);
            DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, paddleColor);
        }

        void update () {
            if (IsKeyDown(KEY_UP)) {
                y -= speed;
            }

            if (IsKeyDown(KEY_DOWN)) {
                y += speed;
            }

            // Limiting Vertical Movement
            if (y <= 0) y = 0;
            if ((y + height) >= GetScreenHeight()) y = GetScreenHeight() - height;
        }
        
        
        float getY () {
            return y;
        }

        float getX () {
            return x;
        }

        float getWidth () {
            return width;
        }

        float getHeight () {
            return height;
        }

        void setSpeed (float s) {
            speed = s;
        }
};

class AiPaddle : public Paddle {
    public:
        AiPaddle (float x, float y, float w, float h, float s) : Paddle (x, y, w, h, s) {}

        void update (int ballY) {
            if ((y + height / 2) > ballY) y -= speed;
            if ((y + height / 2) <= ballY) y += speed;

            // Limiting Vertical Movement
            if (y <= 0) y = 0;
            if ((y + height) >= GetScreenHeight()) y = GetScreenHeight() - height;
        }
};

class PongCity {
    public:
        enum PongState {
            GAME_MENU,
            GAME_PLAYING,
            GAME_WIN,
            GAME_LOSE
        };

        Ball *ball;
        Paddle *player;
        AiPaddle *ai;
        bool isPaused;
        
        Texture2D menu;
        Texture2D karachi;
        Texture2D newYork;
        Texture2D rome;
        Texture2D win;
        Texture2D lose;
        Texture2D pause;

        PongState pongState;

        PongCity () {
            pongState = GAME_MENU;
            isPaused = false;

            ball = new Ball (GetScreenWidth() / 2, GetScreenHeight() / 2, 20.0, 12.0, 12.0);
            ai = new AiPaddle (10, GetScreenHeight() / 2 - 60, 30, 200, 8.5);
            player = new Paddle (GetScreenWidth() - 25 - 10, GetScreenHeight() / 2 - 60, 30, 200, 8.0); 

            // Loading Textures
            menu = LoadTexture("assets/pongCity/menu.png");
            karachi = LoadTexture("assets/pongCity/karachi.png");
            newYork = LoadTexture("assets/pongCity/newYork.png");
            rome = LoadTexture("assets/pongCity/rome.png");
            win = LoadTexture("assets/pongCity/win.png");
            lose = LoadTexture("assets/pongCity/lose.png");
            pause = LoadTexture("assets/pongCity/pause.png");

        }  

        ~PongCity () {
            unloadTextures();
            delete ball; 
            delete ai; 
            delete player;
        }

        void unloadTextures () {
            UnloadTexture(menu);
            UnloadTexture(karachi);
            UnloadTexture(newYork);
            UnloadTexture(win);
            UnloadTexture(lose);
            UnloadTexture(pause);
        }

        void draw () {
            if (pongState == GAME_MENU) {
                DrawTexture(menu, 0, 0, WHITE);
            }

            if (pongState == GAME_PLAYING) {
                if (isPaused) {
                    DrawTexture (pause, 0, 0, WHITE);
                    if (IsKeyPressed(KEY_X)) {
                        isPaused = !isPaused;
                    }
                    if (IsKeyPressed(KEY_M)) {
                        pongState = GAME_MENU;
                        isPaused = false;
                    }
                    if (IsKeyPressed(KEY_R)) {
                        aiScore = 0;
                        playerScore = 0;
                        isPaused = false;
                    }
                } else {
                    if (gameLevel == 1) {
                        DrawTexture(karachi, 0, 0, WHITE);
                        DrawText("KARACHI", WINDOW_WIDTH / 2 - 160, 20, 80, BLACK);
                        DrawText(TextFormat("%i", aiScore), WINDOW_WIDTH / 5 - 20, 20, 80, BLACK);
                        DrawText( TextFormat("%i", playerScore), 4 * WINDOW_WIDTH / 5 - 20, 20, 80, BLACK);
    
                    } else if (gameLevel == 2) {
                        DrawTexture(newYork, 0, 0, WHITE); 
                        DrawText("NEW YORK", WINDOW_WIDTH / 2 - 200, 20, 80, BLACK);
                        DrawText(TextFormat("%i", aiScore), WINDOW_WIDTH / 5 - 20, 20, 80, BLACK);
                        DrawText(TextFormat("%i", playerScore), 4 * WINDOW_WIDTH / 5 - 20, 20, 80, BLACK);
                        
                        // increasing speed of objects
                        static bool speedSet = false;
                        if (!speedSet) {
                            ball->setSpeedX(15.0);
                            ball->setSpeedY(15.0);
                            ai->setSpeed(12);
                            player->setSpeed(10);
                            speedSet = true;
                        }
    
                    } else if (gameLevel == 3) {
                        DrawTexture(rome, 0, 0, WHITE); 
                        DrawText("ROME", WINDOW_WIDTH / 2 - 120, 20, 80, BLACK);
                        DrawText(TextFormat("%i", aiScore), WINDOW_WIDTH / 5 - 20, 20, 80, BLACK);
                        DrawText(TextFormat("%i", playerScore), 4 * WINDOW_WIDTH / 5 - 20, 20, 80, BLACK);
                        
                        // increasing speed of objects
                        static bool speedSet = false;
                        if (!speedSet) {
                            ball->setSpeedX(20.0);
                            ball->setSpeedY(18.0);
                            ai->setSpeed(14);
                            player->setSpeed(12);
                            speedSet = true;
                        }
                    }                
                    ball->draw();
                    player->draw();
                    ai->draw();
                }
                
            }

            if (pongState == GAME_WIN) {
                DrawTexture (win, 0, 0, WHITE);
            }

            if (pongState == GAME_LOSE) {
                DrawTexture (lose, 0, 0, WHITE);
            }
        }

        void update () {
            if (pongState == GAME_MENU) {
                if (IsKeyPressed(KEY_K)) {
                    gameLevel = 1;
                    pongState = GAME_PLAYING;
                }
                if (IsKeyPressed(KEY_N)) {
                    gameLevel = 2;
                    pongState = GAME_PLAYING;
                }
                if (IsKeyPressed(KEY_R)) {
                    gameLevel = 3;
                    pongState = GAME_PLAYING;
                }
                aiScore = 0;
                playerScore = 0;
            }

            if (pongState == GAME_PLAYING && !isPaused) {
                if (IsKeyPressed(KEY_P)) isPaused = !isPaused;

                ball->update();
                player->update();
                ai->update(ball->getY());

                // Checking For Paddle - Ball Hits
                if (CheckCollisionCircleRec({ball->getX(), ball->getY()}, ball->getRadius(), {player->getX(), player->getY(), player->getWidth(), player->getHeight()})) {
                    ball->InvertSpeedX();
                }

                if (CheckCollisionCircleRec({ball->getX(), ball->getY()}, ball->getRadius(), {ai->getX(), ai->getY(), ai->getWidth(), ai->getHeight()})) {
                    ball->InvertSpeedX();
                }

                // winning conditions
                if (gameLevel == 1) {
                    if (playerScore == 3) pongState = GAME_WIN;
                    if (aiScore == 3) pongState = GAME_LOSE;
    
                } else if (gameLevel == 2) {
                    if (playerScore == 5) pongState = GAME_WIN;
                    if (aiScore == 5) pongState = GAME_LOSE;
    
                } else if (gameLevel == 3) {
                    if (playerScore == 7) pongState = GAME_WIN;
                    if (aiScore == 7) pongState = GAME_LOSE;
                }
            }

            if (pongState == GAME_WIN) {
                playerScore = 0;
                aiScore = 0;
                if (IsKeyPressed(KEY_M)) pongState = GAME_MENU;
                if (IsKeyPressed(KEY_R)) pongState = GAME_PLAYING;
            }
        
            if (pongState == GAME_LOSE) {
                playerScore = 0;
                aiScore = 0;
                if (IsKeyPressed(KEY_M)) pongState = GAME_MENU;
                if (IsKeyPressed(KEY_R)) pongState = GAME_PLAYING;
            }
        }
};

#endif