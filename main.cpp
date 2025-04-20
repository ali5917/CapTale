#include <iostream>
#include "C:\raylib\raylib\src\raylib.h"
#include <fstream>

#include "headers\customCity.h"
#include "headers\lobby.h"
#include "headers\pongCity.h"
#include "headers\carCity.h"

using namespace std;

class CapTaleSystem {
    public:
        enum CapTaleState {
            LOBBY,
            CUSTOM_CITY, 
            PONG_CITY,
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
        Lobby lobby;
        Cap player;

    public:
        CapTaleSystem () {
            // state = CUSTOM_CITY;
            state = CAR_CITY;
        }

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
                    // 
                    state = LOBBY;
                }
            } else if (state == LOBBY) {
               
            } else if (state == PONG_CITY) {
                if (IsKeyPressed(KEY_L)) {
                    state = LOBBY;
                }
                pongCity.update();
            } else if (state == CAR_CITY) {
                if (IsKeyPressed(KEY_L)) {
                    state = LOBBY;
                }
                float d = GetFrameTime();
                carCity.update(d);
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
            }
    
            EndDrawing();
        }
    
};



int main () {
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(screenWidth, screenHeight, "CapTale");
    SetTargetFPS(60);

    CapTaleSystem game;
    game.runGame();
    return 0;
}