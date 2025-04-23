#ifndef ENERGYCITY_H
#define ENERGYCITY_H

#include "C:\raylib\raylib\src\raylib.h"
#include <cstdlib>

class Fruit {
    private:
        Vector2 position;
        Texture2D fruitTexture;
        float speed;
        bool active = false;
    
    public:
        Fruit () {}

        void initialize (const char* path, float s) {
            speed = s;
            fruitTexture = LoadTexture(path);
        }

        void update () {
            if (!active) return;

            position.y += speed;   
            if (position.y >= GetScreenHeight()) {
                active = false;
            } 
        }

        void draw () {
            if (active) {
                DrawTexture(fruitTexture, position.x, position.y, WHITE);
            }
        }

        void spawn () {
            position.x = rand() % (GetScreenWidth() - fruitTexture.width);
            position.y = 0;
            active = true;
        }

        bool isActive() {
            return active;
        }
};

class Basket {

};

class EnergyCity {

    private:
        Texture2D background;
        int numFruits = 5;
        Fruit *fruits;
        float spawnTimer = 0.0f;
        int currentFruit = 0;

    public:
        EnergyCity () {
            background = LoadTexture("assets/energyCity/bg1.png");

            fruits = new Fruit [numFruits];

            for (int i = 0; i < numFruits; i++) {
                fruits[i].initialize("assets/energyCity/apple.png", 5.0f);
            }
        }    

        ~EnergyCity () {
            UnloadTexture(background);
            delete [] fruits;
        }

        void draw () {
            DrawTexture(background, 0, 0, WHITE);
            for (int i = 0; i < numFruits; i++) {
                fruits[i].draw();
            }
        }

        void update () {
            spawnTimer += GetFrameTime();
            
            if (spawnTimer >= 1.0f) {
                spawnTimer = 0.0f;
            
                for (int i = 0; i < numFruits; i++) {
                    if (!fruits[i].isActive()) {
                        fruits[i].spawn();
                        break;
                    }
                }
            }
            
            for (int i = 0; i < numFruits; i++) {
                fruits[i].update();
            }
        }
};

#endif