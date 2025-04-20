#ifndef CARCITY_H
#define CARCITY_H

#include "C:\raylib\raylib\src\raylib.h"

// Draw text with a drop shadow for readability
void DrawTextWithShadow(const char* text, int x, int y, int fontSize, Color mainColor, Color shadowColor = BLACK) {
    DrawText(text, x + 2, y + 2, fontSize, shadowColor);
    DrawText(text, x, y, fontSize, mainColor);
}

class Car {
public:
    Texture2D texture;
    Vector2 position;
    float speed;
    bool frozen;
    Car(const char* path) {
        texture = LoadTexture(path);
        position = { (GetScreenWidth() - texture.width)/2.0f, GetScreenHeight() - 150.0f };
        speed = 8.0f;
        frozen = false;
    }
    void Update() {
        if (frozen) return;
        if (IsKeyDown(KEY_LEFT))  position.x -= speed;
        if (IsKeyDown(KEY_RIGHT)) position.x += speed;
        // Clamp
        if (position.x < 0) position.x = 0;
        if (position.x > GetScreenWidth() - texture.width) position.x = GetScreenWidth() - texture.width;
    }
    void Draw() const {
        DrawTexture(texture, (int)position.x, (int)position.y, WHITE);
    }
    Rectangle GetRect() const {
        return { position.x, position.y, (float)texture.width, (float)texture.height };
    }
    void Unload() {
        UnloadTexture(texture);
    }
};

class Obstacle {
public:
    Texture2D texture;
    Rectangle rect;
    bool isBig, active;
    Obstacle(): active(false) {}
    void Spawn(Texture2D banana, Texture2D tree) {
        isBig = GetRandomValue(0, 100) > 50;
        texture = isBig ? tree : banana;
        rect.x = (float)GetRandomValue(0, GetScreenWidth() - texture.width);
        rect.y = -(float)texture.height;
        rect.width  = (float)texture.width;
        rect.height = (float)texture.height;
        active = true;
    }
    void Update(float speed) {
        if (!active) return;
        rect.y += speed;
        if (rect.y > GetScreenHeight()) active = false;
    }
    void Draw() const {
        if (active) DrawTexture(texture, (int)rect.x, (int)rect.y, WHITE);
    }
};

class Coin {
public:
    static constexpr float COIN_SIZE = 31.5f; 

    Rectangle rect;
    bool active;

    Coin(): active(false) {}

    void Spawn() {
        rect.x      = (float)GetRandomValue(0, GetScreenWidth() - (int)COIN_SIZE);
        rect.y      = -COIN_SIZE;  
        rect.width  = COIN_SIZE;
        rect.height = COIN_SIZE;
        active = true;
    }

    void Update(float speed) {
        if (!active) return;
        rect.y += speed;
        if (rect.y > GetScreenHeight()) active = false;
    }

    void Draw() const {
        if (!active) return;
        DrawCircle(
            (int)(rect.x + COIN_SIZE / 2),
            (int)(rect.y + COIN_SIZE / 2),
            COIN_SIZE / 2,  
            GOLD
        );
    }
};

class PowerUp {
public:
    Texture2D texture;
    Rectangle rect;
    bool active;
    int type; // 0=slow,1=freeze,2=coin rain
    PowerUp(): active(false) {}
    void Spawn(Texture2D red, Texture2D blue, Texture2D green) {
        type = GetRandomValue(0,2);
        texture = (type==0?red:(type==1?blue:green));
        rect.x = (float)GetRandomValue(0, GetScreenWidth() - texture.width);
        rect.y = -(float)texture.height;
        rect.width  = (float)texture.width;
        rect.height = (float)texture.height;
        active = true;
    }
    void Update(float speed) {
        if (!active) return;
        rect.y += speed;
        if (rect.y > GetScreenHeight()) active = false;
    }
    void Draw(float size) const {
        if (!active) return;
        Rectangle src = {0,0,(float)texture.width,(float)texture.height};
        Rectangle dst = {rect.x,rect.y,size,size};
        DrawTexturePro(texture, src, dst, {0,0}, 0.0f, WHITE);
    }
};

class Background {
public:
    Texture2D texture;
    float scrollY;
    Background(const char* path) {
        texture = LoadTexture(path);
        scrollY = 0;
    }
    void Update(float d) {
        scrollY += 200 * d;
        if (scrollY >= texture.height) scrollY = 0;
    }
    void Draw() const {
        DrawTexture(texture, 0, (int)(scrollY - texture.height), WHITE);
        DrawTexture(texture, 0, (int)scrollY, WHITE);
    }
    void Unload() {
        UnloadTexture(texture);
    }
};

class CarCity {
    Car* car;
    Background* bg;
    Texture2D banana, tree, pRed, pBlue, pGreen;

    static const int MAX_OBS   = 100;
    static const int MAX_COINS = 200;
    static const int MAX_PUPS  = 20;

    Obstacle obs[MAX_OBS];
    Coin     coins[MAX_COINS];
    PowerUp  pups[MAX_PUPS];

    int   oCount=0, pCount=0;
    int   score=0, coinsCollected=0, highScore=0;
    float scoreTimer=0, obsTimer=0, coinTimer=0, pupTimer=0, diffTimer=0;

    bool  gameOver=false, paused=false, usedSecond=false, showLife=false;
    float pauseTimer=0;
    float obsSpeed=6.0f, coinSpeed=5.0f;

    // effect timers
    float effectTimer=0;

    // coin‐rain specific
    bool  coinRainActive        = false;
    float coinRainDuration      = 0;
    float coinRainBurstTimer    = 0;

    std::string effectMsg;

public:
    CarCity() {
        car    = new Car("assets/carCity/car.png");
        bg     = new Background("assets/carCity/background.png");
        banana = LoadTexture("assets/carCity/banana.png");
        tree   = LoadTexture("assets/carCity/tree.png");
        pRed   = LoadTexture("assets/carCity/power_red.png");
        pBlue  = LoadTexture("assets/carCity/power_blue.png");
        pGreen = LoadTexture("assets/carCity/power_green.png");
        srand((unsigned)time(NULL));
    }
    ~CarCity() {
        car->Unload();    delete car;
        bg->Unload();     delete bg;
        UnloadTexture(banana);
        UnloadTexture(tree);
        UnloadTexture(pRed);
        UnloadTexture(pBlue);
        UnloadTexture(pGreen);
    }

    void update(float d) {
        // Restart logic
        if (gameOver) {
            if (IsKeyPressed(KEY_R)) Restart();
            return;
        }
        // Second life pause
        if (paused) {
            pauseTimer -= d;
            if (pauseTimer <= 0) {
                paused   = false;
                showLife = false;
            }
            return;
        }

        bg->Update(d);
        car->Update();

        // Difficulty ramp every 15s
        diffTimer += d;
        if (diffTimer >= 15.0f) {
            diffTimer = 0;
            obsSpeed += 1;
            coinSpeed+= 0.5f;
            car->speed += 0.5f;
        }

        // Spawn obstacles
        obsTimer += d;
        if (obsTimer >= 1.5f && oCount < MAX_OBS) {
            obsTimer = 0;
            obs[oCount++].Spawn(banana, tree);
        }

        // Normal coin spawn every 2s
        coinTimer += d;
        if (coinTimer >= 2.0f) {
            coinTimer = 0;
            for (int i = 0; i < MAX_COINS; i++) {
                if (!coins[i].active) {
                    coins[i].Spawn();
                    break;
                }
            }
        }

        // Spawn power-ups
        pupTimer += d;
        if (pupTimer >= 8.0f && pCount < MAX_PUPS) {
            pupTimer = 0;
            pups[pCount++].Spawn(pRed, pBlue, pGreen);
        }

        // Update obstacles & collisions
        for (int i = 0; i < oCount; i++) {
            if (!obs[i].active) continue;
            obs[i].Update(obsSpeed);
            if (CheckCollisionRecs(obs[i].rect, car->GetRect())) {
                obs[i].active = false;
                if (obs[i].isBig || usedSecond) {
                    gameOver = true;
                    if (score > highScore) highScore = score;
                } else {
                    paused     = true;
                    pauseTimer = 2.0f;
                    showLife   = true;
                    usedSecond = true;
                }
            }
        }

        // Update coins & collection
        for (int i = 0; i < MAX_COINS; i++) {
            if (!coins[i].active) continue;
            coins[i].Update(coinSpeed);
            if (CheckCollisionRecs(coins[i].rect, car->GetRect())) {
                coins[i].active = false;
                coinsCollected++;
            }
        }

        // Update power-ups & apply effects
        for (int i = 0; i < pCount; i++) {
            if (!pups[i].active) continue;
            pups[i].Update(obsSpeed);
            if (CheckCollisionRecs(pups[i].rect, car->GetRect())) {
                pups[i].active = false;
                effectTimer    = 0;
                switch (pups[i].type) {
                    case 0: // slow
                        car->speed = 3.0f;
                        effectMsg  = "SLOW MODE!";
                        break;
                    case 1: // freeze
                        car->frozen = true;
                        effectMsg   = "FROZEN!";
                        break;
                    case 2: // coin rain
                        coinRainActive     = true;
                        coinRainDuration   = 0;
                        coinRainBurstTimer = 0;
                        effectMsg          = "COIN RAIN!";
                        break;
                }
            }
        }

        // Handle slow/freeze effect end (5s)
        if (!coinRainActive && !effectMsg.empty()) {
            effectTimer += d;
            if (effectTimer > 5.0f) {
                effectMsg   .clear();
                car->speed  = 8.0f;
                car->frozen = false;
            }
        }

        // Handle coin rain bursts (5s)
        if (coinRainActive) {
            coinRainDuration   += d;
            coinRainBurstTimer += d;
            if (coinRainBurstTimer >= 0.1f) {
                coinRainBurstTimer = 0;
                // spawn 5 coins each burst
                for (int b = 0; b < 5; b++) {
                    for (int j = 0; j < MAX_COINS; j++) {
                        if (!coins[j].active) {
                            coins[j].Spawn();
                            break;
                        }
                    }
                }
            }
            if (coinRainDuration >= 5.0f) {
                coinRainActive = false;
                effectMsg.clear();
            }
        }

        // Increase score every second
        scoreTimer += d;
        if (scoreTimer >= 1.0f) {
            scoreTimer = 0;
            score++;
        }
    }

    void draw() const {
        bg->Draw();
        car->Draw();

        for (int i = 0; i < oCount; i++)
            if (obs[i].active) obs[i].Draw();

        for (int i = 0; i < MAX_COINS; i++)
            if (coins[i].active) coins[i].Draw();

        Rectangle src = {0,0,(float)pRed.width,(float)pRed.height};
        for (int i = 0; i < pCount; i++) {
            if (pups[i].active) {
                Rectangle dst = { pups[i].rect.x, pups[i].rect.y, (float)pups[i].texture.width, (float)pups[i].texture.height };
                DrawTexturePro(pups[i].texture, src, dst, {0,0}, 0, WHITE);
            }
        }

        // UI: Score and coins
        std::string s = "Score: " + std::to_string(score);
        std::string c = "Coins: " + std::to_string(coinsCollected);
        int x = GetScreenWidth()/2 - MeasureText(s.c_str(),30)/2;
        DrawTextWithShadow(s.c_str(), x, 10, 30, YELLOW);
        DrawTextWithShadow(c.c_str(), x, 45, 30, ORANGE);

        // Second life notification
        if (showLife) {
            const char* m = "Second Life!";
            DrawTextWithShadow(m, GetScreenWidth()/2 - MeasureText(m,40)/2, GetScreenHeight()/2 - 80, 40, GREEN);
        }

        // Effect message (slow/freeze or coin rain)
        if (!effectMsg.empty()) {
            DrawTextWithShadow(effectMsg.c_str(), GetScreenWidth()/2 - MeasureText(effectMsg.c_str(),50)/2, 100, 50, SKYBLUE);
        }

        // Game over screen
        if (gameOver) {
            DrawTextWithShadow("GAME OVER", GetScreenWidth()/2 - MeasureText("GAME OVER",70)/2, GetScreenHeight()/2 - 120, 70, RED);

            DrawTextWithShadow(
                TextFormat("Final Score: %d", score), GetScreenWidth()/2 - 100, GetScreenHeight()/2 - 50, 35, WHITE);

            DrawTextWithShadow(
                TextFormat("High Score: %d", highScore), GetScreenWidth()/2 - 100, GetScreenHeight()/2, 35, GREEN);

            DrawTextWithShadow(
                TextFormat("Coins: %d", coinsCollected), GetScreenWidth()/2 - 100, GetScreenHeight()/2 + 50, 35, GOLD);

            DrawTextWithShadow("Press R to Restart", GetScreenWidth()/2 - 100, GetScreenHeight()/2 + 100, 25, LIGHTGRAY);
            DrawTextWithShadow("Press L to return to Lobby", GetScreenWidth()/2 - 275, GetScreenHeight()/2 + 150, 40, BLUE);
        }
    }

    void Restart() {
        oCount = 0;
        pCount = 0;
        // deactivate all coins
        for (int i = 0; i < MAX_COINS; i++) coins[i].active = false;

        score = coinsCollected = 0;
        scoreTimer = obsTimer = coinTimer = pupTimer = diffTimer = pauseTimer = effectTimer = 0;
        gameOver = paused = usedSecond = showLife = coinRainActive = false;
        obsSpeed = 6.0f;
        coinSpeed = 5.0f;
        effectMsg.clear();

        car->speed  = 8.0f;
        car->frozen = false;
        car->position = { (GetScreenWidth() - car->texture.width)/2.0f, GetScreenHeight() - 150.0f };
    }

};

#endif