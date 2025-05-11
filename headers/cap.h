#ifndef CAP_H
#define CAP_H

#include "C:\raylib\raylib\src\raylib.h"
#include "settings.h"

Vector2 normalized(Vector2 v) {
    if (v.x != 0 && v.y != 0) {
        if (v.x < 0) v.x = -1;
        else if (v.x > 0) v.x = 1;

        if (v.y < 0) v.y = -1;
        else if (v.y > 0) v.y = 1;
    }
    return v;
}

class Cap {
    private:
        Texture texture;
        Vector2 pos;
        int speed;
        Vector2 direction;
        Vector2 size;
        bool discard;
        float collisionRadius;
        int tokens;
        int cash;
        int energy;
        bool gameOver;
        int stepsMoved = 0;
        const int stepsPerDrain = 25;
        const int energyPerDrain = 1;
    public:
        Cap(Vector2 p={0,0}, int s=CAP_SPEED, Vector2 d={0,0}, Texture t=LoadTexture("assets/customCity/cap0.png")) : texture(t), pos(p), speed(s), direction(d), size({(float)t.width, (float)t.height}), discard(false), tokens(0), cash(INITIAL_CASH), energy(75), gameOver(false) {
            collisionRadius = size.y/2;
        }

        void input() {
            direction.x = int(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) - int(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A));
            direction.y = int(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) - int(IsKeyDown(KEY_UP) || IsKeyDown(KEY_W));
            direction = normalized(direction);
        }

        void constraint() {
            pos.x = ((pos.x < (WINDOW_WIDTH - size.x)) ? pos.x : (WINDOW_WIDTH - size.x));
            pos.x = (pos.x > 0) ? pos.x : 0;

            pos.y = ((pos.y < (WINDOW_HEIGHT - size.y)) ? pos.y : (WINDOW_HEIGHT - size.y));
            pos.y = (pos.y > 0) ? pos.y : 0;
        }

        ~Cap() {}

        void move(double dt) {
            if (energy <= MIN_ENERGY) {
                gameOver = true;
                return;
            }
        
            Vector2 prevPos = pos;
        
            pos.x += direction.x * speed * dt;
            pos.y += direction.y * speed * dt;
            if ((int)pos.x != (int)prevPos.x || (int)pos.y != (int)prevPos.y) {
                stepsMoved++;
                if (stepsMoved >= stepsPerDrain) {
                    decreaseEnergy(energyPerDrain);
                    stepsMoved = 0;
                }
            }
        }
        

        void update(double dt) {
            input();
            move(dt);
            constraint();
        };

        void checkDiscard() {
            discard = !(-300 < (pos.y + WINDOW_HEIGHT) && (pos.y + WINDOW_HEIGHT) > 300);
        }

        void draw() {
            DrawTextureV(texture, pos, WHITE);
        }

        bool getDiscardStatus() { return discard; }

        Vector2 getPos() { return pos; }
        virtual Vector2 getCenter() {
            return Vector2({pos.x + size.x/2, pos.y + size.y/2});
        }

        float getCollisionRadius() { return collisionRadius; }

        void updateDiscardStatus(bool s) {
            discard = s;
        }
        void setSize(Vector2 s) {
            size = s;
        }
        Vector2 getSize() {
            return size;
        }
        void setTexture(Texture2D tex) {
            texture = tex;
        }
        void setPosition(Vector2 p) {
            pos = p;
        }
        Rectangle getRectangle() {
            return {pos.x,pos.y, size.x, size.y};
        }

        void addTokens(int amount) {
            tokens += amount;
        }
        void removeTokens(int amount) {
            tokens -= amount;
        }

        void addCash(int amount) {
            cash += amount;
        }
        void removeCash(int amount) {
            cash -= amount;
        }

        void increaseEnergy(int amount) {
            if ((energy + amount) < MAX_ENERGY) {
                energy += amount;
            } else {
                energy = MAX_ENERGY;
            }
        }

        void decreaseEnergy(int amount) {
            if ((energy - amount) > MIN_ENERGY) {
                energy -= amount;
            } else {
                energy = MIN_ENERGY;
            }
        }

        void drawDead () {

        }

        int getCash() { return cash; }
        int getTokens() { return tokens; }
        int getEnergy() { return energy; }
        bool getGameOver() { return gameOver; }
        
        friend class ATMCity;
        friend class Lobby;
};

#endif
