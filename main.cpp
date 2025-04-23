#include <iostream>
#include "C:\raylib\raylib\src\raylib.h"
#include <fstream>

#include "headers\customCity.h"
#include "headers\lobby.h"
#include "headers\pongCity.h"
#include "headers\carCity.h"
#include "headers\atmCity.h"
#include "headers\spaceShooter.h"
using namespace std;

void playIntroAnimation() {
    const int totalFrames = 253; // Number of frames you exported
    Texture2D frames[totalFrames];

    SetTargetFPS(0);

    // Load all frames
    for (int i = 0; i < totalFrames; i++) {
        char filename[64];
        sprintf(filename, "assets/intro/frames/intro_frame_%04d.png", i + 1);
        frames[i] = LoadTexture(filename);
    }

    // Show each frame for 1/30th of a second
    for (int i = 0; i < totalFrames; i++) {
        BeginDrawing();
        ClearBackground(WHITE);
        float posX = (WINDOW_WIDTH - frames[i].width) / 2.0f;
        float posY = (WINDOW_HEIGHT - frames[i].height) / 2.0f;
        DrawTexture(frames[i], posX, posY, WHITE);
        EndDrawing();
        WaitTime(1.0f / 30.0f); // 30 FPS
    }

    // Unload textures
    for (int i = 0; i < totalFrames; i++) {
        UnloadTexture(frames[i]);
    }

    SetTargetFPS(60); 

    return;
}

class CapTaleSystem {
    public:
        enum CapTaleState {
            LOBBY,
            CUSTOM_CITY, 
            PONG_CITY,
            ATM_CITY,
            CAR_CITY,
            SHOOTER_CITY,
            MATH_CITY,
            ENERGY_CITY
        };
    
    private:
        CapTaleState state;
        CustomCity customCity;
        PongCity pongCity;
        CarCity carCity;
        ATMCity atmCity;
        Lobby lobby;
        SpaceShooter spaceShooter;
        Cap player = Cap();

        bool enterPong;

    public:
        CapTaleSystem (Texture2D bgTex) : state(CUSTOM_CITY), lobby(&player), enterPong(false) {}
        // CapTaleSystem (Texture2D bgTex) : state(LOBBY), lobby(&player) {}

        ~CapTaleSystem () {
            CloseWindow();
        }

        void runGame () {
            while (WindowShouldClose() == false) {
                update();
                draw();
            }
        }

        void update() {
            if (state == CUSTOM_CITY) {
                if (IsKeyPressed(KEY_RIGHT)) customCity.nextCap();
                if (IsKeyPressed(KEY_LEFT)) customCity.prevCap();
                if (IsKeyPressed(KEY_ENTER)) {
                    Texture lobbyCapTex = LoadTexture(("assets/lobby/" + to_string(customCity.getSelectedCap()) + ".png").c_str());
                    player.setTexture(lobbyCapTex);
                    player.setPosition({WINDOW_WIDTH/2.0f - lobbyCapTex.width/2.0f, WINDOW_HEIGHT - lobbyCapTex.height - 20.0f});
                    player.setSize({(float)lobbyCapTex.width, (float)lobbyCapTex.height});
                    state = LOBBY;
                }
            } else if (state == LOBBY) {
                state = (CapTaleState)lobby.update();
            } else if (state == PONG_CITY) {
                if (IsKeyPressed(KEY_L)) {
                    state = LOBBY;
                    enterPong = false;
                }

                if (!enterPong) {
                    enterPong = true;
                    pongCity.isPaused = false;
                    pongCity.pongState = PongCity::GAME_ENEMY;
                    
                }

                pongCity.update();
                

            } else if (state == CAR_CITY) {
                if (IsKeyPressed(KEY_L)) {
                    state = LOBBY;
                }
                float d = GetFrameTime();
                carCity.update(d);
            } else if (state == ATM_CITY) {
                if (IsKeyPressed(KEY_L)) {
                    state = LOBBY;
                }
                atmCity.update();
               
            } else if (state == SHOOTER_CITY) {
                if (IsKeyPressed(KEY_L)) {
                    spaceShooter.restart();
                    state = LOBBY;
                }
                bool val = spaceShooter.update();
                if(val) {
                    spaceShooter.restart();
                    state = LOBBY;
                }
            }
        }

        void draw () {
            BeginDrawing();
            ClearBackground(BLACK);
           
            if (state == CUSTOM_CITY) {
                customCity.draw();
            } else if (state == LOBBY) {
                lobby.draw();
            } else if (state == PONG_CITY) {
                pongCity.draw();
            } else if (state == CAR_CITY) {
                carCity.draw();
            } else if (state == ATM_CITY) {
                atmCity.draw();
            } else if (state == SHOOTER_CITY) {
                spaceShooter.draw();
            }
            EndDrawing();
        }
    
};



int main () {
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "CapTale");
    SetTargetFPS(60);

    playIntroAnimation(); 

    Texture2D capTex = LoadTexture("assets/cap.png");
    Texture2D bgTex = LoadTexture("assets/lobby/bg1.png");

    CapTaleSystem game(bgTex);
    game.runGame();

    UnloadTexture(capTex);
    UnloadTexture(bgTex);
    return 0;
}
