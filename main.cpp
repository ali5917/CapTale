#include <iostream>
#include "C:\raylib\raylib\src\raylib.h"
#include <fstream>

#include "headers\customCity.h"
#include "headers\lobby.h"

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
            ENERGY_CITY, 
            BANK_CITY
        };
    
    private:
        CapTaleState state;
        CustomCity customCity;
        Lobby lobby;
        Cap player = Cap();

    public:
        CapTaleSystem (Texture2D bgTex) : state(CUSTOM_CITY), lobby(&player) {}

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
            }
        }

        void draw () {
            BeginDrawing();
            ClearBackground(BLACK);
           
            if (state == CUSTOM_CITY) {
                customCity.draw();
            } else if (state == LOBBY) {
                lobby.draw();
            }
    
            EndDrawing();
        }
    
};



int main () {
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "CapTale");
    SetTargetFPS(60);

    Texture2D capTex = LoadTexture("assets/cap.png");
    Texture2D bgTex = LoadTexture("assets/lobby/bg1.png");

    CapTaleSystem game(bgTex);
    game.runGame();

    UnloadTexture(capTex);
    UnloadTexture(bgTex);
    return 0;
}
