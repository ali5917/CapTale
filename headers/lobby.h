#ifndef LOBBY_H
#define LOBBY_H

#include "C:\raylib\raylib\src\raylib.h"
#include "cap.h"

class Lobby {
    private:
        Texture2D background;
        
        Cap *player;
        float capSpeed;

    public:
        Lobby () {
            capSpeed = 4.0f;
            player = nullptr;

            background = LoadTexture("assets/lobby/bg1.png");
        }
        
        ~Lobby () {

        }

        void draw () {
            DrawTexture(background, 0, 0, WHITE);

        }
};

#endif