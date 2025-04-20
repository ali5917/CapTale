#ifndef SPRITES_H
#define SPRITES_H
#include "C:\raylib\raylib\src\raylib.h"
#include <functional>
#include "custom_timer.h"
using namespace std;

Vector2 normalize(Vector2 v) {
    if (v.x != 0 && v.y != 0) {
        if (v.x < 0) v.x = -1;
        else if (v.x > 0) v.x = 1;

        if (v.y < 0) v.y = -1;
        else if (v.y > 0) v.y = 1;
    }
    return v;
}

class Sprite {
    protected:
        Texture texture;
        Vector2 pos;
        int speed;
        Vector2 direction;
        Vector2 size;
        bool discard;
        float collisionRadius;
    public:
        Sprite(Texture t, Vector2 p, int s, Vector2 d) : texture(t), pos(p), speed(s), direction(d), size({(float)t.width, (float)t.height}), discard(false) {
            collisionRadius = size.y/1.5;
        }

        virtual ~Sprite() {}

        virtual void move(double dt) {
            pos.x += direction.x * speed * dt;
            pos.y += direction.y * speed * dt;
        }

        virtual void update(double dt) {
            move(dt);
            checkDiscard();
        };

        void checkDiscard() {
            discard = !(-300 < (pos.y + WINDOW_HEIGHT) && (pos.y + WINDOW_HEIGHT) > 300);
        }

        virtual void draw() {
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
        Vector2 getSize() {
            return size;
        }

};

class Player : public Sprite {
    private:
        void input() {
            direction.x = int(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) - int(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A));
            direction.y = int(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) - int(IsKeyDown(KEY_UP) || IsKeyDown(KEY_W));
            direction = normalize(direction);

            if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(1)) {
                func({pos.x + size.x/2, pos.y - size.y/2});}
        }

        void constraint() {
            pos.x = ((pos.x < (WINDOW_WIDTH - size.x)) ? pos.x : (WINDOW_WIDTH - size.x));
            pos.x = (pos.x > 0) ? pos.x : 0;

            pos.y = ((pos.y < (WINDOW_HEIGHT - size.y)) ? pos.y : (WINDOW_HEIGHT - size.y));
            pos.y = (pos.y > 0) ? pos.y : 0;
        }

        function<void(Vector2)> func;
        bool tripleShot;
        Timer tripleShotTimer = Timer(5, false, false, [this]() {disableTripleShot(); });
        bool shield;
        Timer shieldTimer = Timer(5, false, false, [this]() {disableShield();});
    public:
        Player(Texture t, Vector2 p, function<void(Vector2)> f = nullptr) : Sprite(t, p, PLAYER_SPEED, { 0, 0 }), func(f), tripleShot(false), shield(false) {}

        void update(double dt) override {
            input();
            move(dt);
            constraint();
            // timers
            tripleShotTimer.update();
            shieldTimer.update();
        }

        // POWER UP 
        bool getTripleShotStatus() {
            return tripleShot;
        }
        void disableTripleShot() {
            tripleShot = false;
        }
        void activateTripleShot() {
            tripleShot = true;
            tripleShotTimer.activate();
        }

        // SHIELD
        void activateShield() {
            shield = true;
            shieldTimer.activate();
        }
        
        void disableShield() {
            shield = false;
        }
        
        bool getShieldStatus() {
            return shield;
        }

        void draw() override {
            Sprite::draw();
            if (shield) {
                for(int i = SHIELD_RADIUS_ADDITION/2; i <= SHIELD_RADIUS_ADDITION; i+=2) {
                    DrawCircleLines(getCenter().x, getCenter().y, getCollisionRadius() + i, SKYBLUE);
                }
            }
        }        
        
        void disablePowerUps() {
            tripleShot = false;
        }
};

class Laser : public Sprite {

    private:
        float rotation;
    public:
        Laser(Texture t, Vector2 pos) : Sprite(t, pos, LASER_SPEED, { 0, -1 }), rotation(0) {}
        Laser(Texture t, Vector2 pos, Vector2 d, float r) : Sprite(t, pos, LASER_SPEED, normalize(d)), rotation(r) {}
        void draw() {
            if (rotation != 0) {
                DrawTextureEx(texture, {pos.x + size.x/4, pos.y}, rotation, 1, WHITE);
            } else {
                DrawTextureV(texture, {pos.x + size.x/4, pos.y}, WHITE);
            }
            
        }
        Rectangle getRect() {
            Rectangle rect = {pos.x, pos.y, size.x, size.y};
            return rect;
        }
        
};

class Meteor : public Sprite {

    private:
        float rotation = 0;
        Rectangle rect;
    public:
        Meteor(Texture t) : Sprite(t, {(float)(rand() % (WINDOW_WIDTH + 1)), (float)(-150 + rand() % (-50-(-150) + 1))}, (METEOR_SPEED_MIN + rand() % (METEOR_SPEED_MAX - METEOR_SPEED_MIN + 1)), {(float)(-0.5 + (rand() / (RAND_MAX + 1.0)) * (0.5 - (-0.5))), 1}) {
            rect = {0,0,size.x,size.y};
        }
        void update(double dt) override {
            Sprite::update(dt);
            rotation += 50 * dt;
        }
        void draw() {
            Rectangle targetRect = {pos.x, pos.y,size.x, size.y};
            DrawTexturePro(texture, rect, targetRect, {size.x/2, size.y/2}, rotation, WHITE);
        }
        Vector2 getCenter() {
            return pos;
        }

};

class ExplosionAnimation {

    private:
        Texture** textures;
        int numTextures;
        Vector2 pos;
        Vector2 size;
        float index;
        bool discard;
    public:
        ExplosionAnimation(Texture** t, int nT, Vector2 pos) : textures(t), numTextures(nT), index(0), discard(false) {
            Vector2 s = {(float)textures[0]->width, (float)textures[0]->height};
            size = s;
            s = {pos.x - size.x/2, pos.y - size.y/2};
            this->pos = s;
        }
        void draw() {
            DrawTextureV(*textures[(int)index], pos, WHITE);
        }
        void update(double dt) {
            if(index < numTextures - 1) {
                index += 20 * dt;
            } else {
                discard = true;
            }   
        }
        bool getDiscardStatus() { return discard; }

};

class PoweredUp : public Sprite {

    protected:
        Player * player;
        bool activated;
        Timer discardTimer = Timer(POWER_DISCARD_TIMER, false, true, [this]() { discardPowerUp(); });
    public:
        PoweredUp(Texture t, Player* p) : Sprite(t, {(float)(rand() % (WINDOW_WIDTH - 100) + 50), (float)(rand() % (WINDOW_HEIGHT/2) + 200)}, 0, {0,0}), player(p), activated(false) {}
        virtual void applyPower() = 0;
        void discardPowerUp() {
            if(!discard) {
                discard = true;
            }
        }
        void update(double dt) override{
            Sprite::update(dt);
            discardTimer.update();
        }
        void move(double dt) override {
            pos.x += direction.x * speed * dt;
        }
        
};

class TripleShot : public PoweredUp {

    public:
        TripleShot(Texture t, Player * p) : PoweredUp(t,p) {}
        void applyPower() {
            player->activateTripleShot();
        }

};

class Shield : public PoweredUp {

    public:
        Shield(Texture t, Player * p) : PoweredUp(t, p) {}
        void applyPower() override {
            player->activateShield();
        }

};


#endif
