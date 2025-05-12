#ifndef SPACESHOOTER_H
#define SPACESHOOTER_H

#include "C:\raylib\raylib\src\raylib.h"
#include "sprites.h"

class Star {
    private:
        Vector2 pos; 
        float size;
    public:
        Star() {
            pos.x = rand() % 1921;
            pos.y = rand() % 1081;
            size = 0.5 + (rand() / (RAND_MAX + 1.0)) * (1.2 - 0.4);
        }
        Vector2 getPos() { return pos; }
        float getSize() { return size; }

};

class SpaceShooter {
    private:
        Texture assets[32];
        enum AssetKey { PLAYER, STAR, LASER,  METEOR, TRIPLESHOT, SHIELD, EXPLOSION};
        Sound sounds[3];
        enum AudioKey {LASERAUDIO, EXPLOSIONAUDIO, POWERUPAUDIO};
        Music BGmusic;
        Font font;
        Player* player;
        Star stars[NUM_STARS];
        Laser** lasers;
        int numLasers;
        int lasersUsed;
        Meteor** meteors;
        int numMeteors;
        ExplosionAnimation ** explosions;
        int numExplosions;
        Timer meteorTimer = Timer(METEOR_TIMER_DURATION, true, true, [this]() { createMeteor(); });
        bool gameOver = false;
        int score;
        PoweredUp ** powerUps;
        int numPowerUps;
        Timer powerUpTimer = Timer(0); //Timer(POWERUP_TIMER_DURATION, true, true, [this]() { createPowerUp(); });
        int tripleShotsCounter;
        int shieldCounter;
        enum GameModes {HUNDREDSECONDS, SIXTYLASERS, FREESTYLE_NOPOWERUPS};
        int selectedGameMode;
        bool selected;
        Timer overTimer = Timer(0);
        double timeSurvived;
        bool returnToLobby;
    public:
        SpaceShooter() : lasers(nullptr), numLasers(0), lasersUsed(0), meteors(nullptr), numMeteors(0), explosions(nullptr), numExplosions(0), score(0), powerUps(nullptr), numPowerUps(0), tripleShotsCounter(0), shieldCounter(0), selectedGameMode(-1), selected(false), timeSurvived(-1), returnToLobby(false) {
            InitAudioDevice();
            importAssets();
            player = new Player(assets[PLAYER],{WINDOW_WIDTH/2, WINDOW_HEIGHT/2}, [this](Vector2 pos) {shootLaser(pos);});
            PlayMusicStream(BGmusic);
        }

        // IMPORT & CREATE SPRITES
        void importAssets() {
            assets[PLAYER] =  LoadTexture("assets/spaceShooter/images/spaceship.png");
            assets[STAR] = LoadTexture("assets/spaceShooter/images/star.png");
            assets[LASER] = LoadTexture("assets/spaceShooter/images/laser.png");
            assets[METEOR] = LoadTexture("assets/spaceShooter/images/meteor.png");
            assets[TRIPLESHOT] = LoadTexture("assets/spaceShooter/images/tripleshot.png");
            assets[SHIELD] = LoadTexture("assets/spaceShooter/images/shield.png");
            for(int i = 0; i < NUM_EXPLOSION_FRAMES; i++) {
                string filename = "assets/spaceShooter/images/explosion/";
                filename += to_string(i + 1);  // converts 0 to '1', 1 to '2', ...
                filename += ".png";
                assets[EXPLOSION + i] = LoadTexture(filename.c_str());
            }
            font = LoadFontEx("assets/spaceShooter/font/Stormfaze.otf", FONT_SIZE, NULL, 0);

            sounds[EXPLOSIONAUDIO] = LoadSound("assets/spaceShooter/audio/explosion.wav");
            sounds[LASERAUDIO] = LoadSound("assets/spaceShooter/audio/laser.wav");
            sounds[POWERUPAUDIO] = LoadSound("assets/spaceShooter/audio/powerup.mp3");
            BGmusic = LoadMusicStream("assets/spaceShooter/audio/music.wav");
        } 

        void createPowerUp() {

            PoweredUp** tempPowerUps = new PoweredUp*[numPowerUps + 1];
            for(int i = 0; i < numPowerUps; i++) {
                tempPowerUps[i] = powerUps[i];
            }
            int choice = rand() % NUM_AVAILABLE_POWERUPS;
            switch (choice) {
                case 0:                    
                    tempPowerUps[numPowerUps] = new TripleShot(assets[TRIPLESHOT], player);
                    break;
                case 1:
                    tempPowerUps[numPowerUps] = new Shield(assets[SHIELD], player);
                    break;
            }
            
            //tempPowerUps[numPowerUps] = new PowerUp(assets[METEOR]);
            delete[] powerUps;
            powerUps = tempPowerUps;
            numPowerUps++;
        }

        void shootLaser(Vector2 pos) {
            int createLasers = 1;
            if(player->getTripleShotStatus()) {
                createLasers = 3;
            }
            Laser** tempLasers = new Laser*[numLasers + createLasers];
            for(int i = 0; i < numLasers; i++) {
                tempLasers[i] = lasers[i];
            }
            tempLasers[numLasers] = new Laser(assets[LASER], pos);
            if(createLasers == 3) {
                tempLasers[numLasers + 1] = new Laser(assets[LASER], {pos.x - 40, pos.y}, {-1, -1}, -45);
                tempLasers[numLasers + 2] = new Laser(assets[LASER], {pos.x + 40, pos.y}, {1, -1}, 45);
                tripleShotsCounter++;
            }
            delete[] lasers;
            lasers = tempLasers;
            numLasers += createLasers;
            lasersUsed++;
            if(selectedGameMode == SIXTYLASERS && lasersUsed >= MAX_LASERS) {
                updateGameOver();
            }
            PlaySound(sounds[LASERAUDIO]);
        }

        void createMeteor() {
            Meteor** tempMeteors = new Meteor*[numMeteors + 1];
            for(int i = 0; i < numMeteors; i++) {
                tempMeteors[i] = meteors[i];
            }
            tempMeteors[numMeteors] = new Meteor(assets[METEOR]);
            delete[] meteors;
            meteors = tempMeteors;
            numMeteors++;
        }

        void createExplosion(Vector2 laserPos, Vector2 laserSize) {
            ExplosionAnimation** tempExplosions = new ExplosionAnimation*[numExplosions + 1];
            for(int i = 0; i < numExplosions; i++) {
                tempExplosions[i] = explosions[i];
            }

            Texture** explosionTextures = new Texture*[NUM_EXPLOSION_FRAMES];
            for(int i = 0; i < NUM_EXPLOSION_FRAMES; i++) {
                explosionTextures[i] = &assets[EXPLOSION + i];
            }
            Vector2 pos = {laserPos.x - laserSize.x/2, laserPos.y};
            tempExplosions[numExplosions] = new ExplosionAnimation(explosionTextures,NUM_EXPLOSION_FRAMES, pos);
            delete[] explosions;
            explosions = tempExplosions;
            numExplosions++;
            PlaySound(sounds[EXPLOSIONAUDIO]);
        }

        // UPDATE!!
        void checkCollisions() {

            // laser & meteor
            for(int i = 0; i < numLasers; i++) {
                for(int j = 0; j < numMeteors; j++) {
                    if(CheckCollisionCircleRec(meteors[j]->getCenter(), meteors[j]->getCollisionRadius(), lasers[i]->getRect())) {
                        lasers[i]->updateDiscardStatus(true);
                        meteors[j]->updateDiscardStatus(true);
                        createExplosion(lasers[i]->getPos(), lasers[i]->getSize());
                        score++;

                    }
                }
            }
            
            // laser & powerUp
            for(int i = 0; i < numLasers; i++) {
                for(int j = 0; j < numPowerUps; j++) {
                    if(CheckCollisionCircleRec(powerUps[j]->getCenter(), powerUps[j]->getCollisionRadius(), lasers[i]->getRect())) {
                        lasers[i]->updateDiscardStatus(true);
                        powerUps[j]->applyPower();
                        powerUps[j]->updateDiscardStatus(true);
                        createExplosion(lasers[i]->getPos(), lasers[i]->getSize());
                        PlaySound(sounds[POWERUPAUDIO]);
                    }
                }
            }

            // player & meteor
            for(int i = 0; i < numMeteors; i++) {
                if(player->getShieldStatus()) {
                    if(CheckCollisionCircles(player->getCenter(), player->getCollisionRadius() + SHIELD_RADIUS_ADDITION, meteors[i]->getCenter(), meteors[i]->getCollisionRadius())) {
                        meteors[i]->updateDiscardStatus(true);
                        createExplosion(meteors[i]->getPos(), meteors[i]->getSize());
                        shieldCounter++;
                    }
                } else {
                    if(CheckCollisionCircles(player->getCenter(), player->getCollisionRadius(), meteors[i]->getCenter(), meteors[i]->getCollisionRadius())) {
                        gameOver = true;
                    }
                }
            }

            // player & powerUp
            for(int i = 0; i < numPowerUps; i++) {
                if(CheckCollisionCircles(player->getCenter(), player->getCollisionRadius(), powerUps[i]->getCenter(), powerUps[i]->getCollisionRadius())) {
                    powerUps[i]->applyPower();
                    powerUps[i]->updateDiscardStatus(true);
                    PlaySound(sounds[POWERUPAUDIO]);
                }
            }

        } 

        bool update() {
            if(returnToLobby) {
                return true;
            }

            if(!selected) {
                return false;
            } else if(!gameOver) {
                float dt = GetFrameTime();
                meteorTimer.update();
                powerUpTimer.update();
                player->update(dt);
                discardSprites();
                UpdateMusicStream(BGmusic);
                for(int i = 0; i < numLasers; i++) {
                    lasers[i]->update(dt);
                }
                for(int i = 0; i < numMeteors; i++) {
                    meteors[i]->update(dt);
                }
                for(int i = 0; i < numExplosions; i++) {
                    explosions[i]->update(dt);
                }
                for(int i = 0; i < numPowerUps; i++) {
                    powerUps[i]->update(dt);
                }
                checkCollisions();
                return false;
            } else if(gameOver) {
                return false;
            }
            return false;
        }

        // DRAW!!
        void drawStars() {
            for(int i = 0; i < NUM_STARS; i++) {
                DrawTextureEx(assets[STAR], stars[i].getPos(), 0, stars[i].getSize(), WHITE);
            }
        }

        void draw() {

            if(returnToLobby) {
                return;
            }

            if(!selected) {
                ClearBackground(BG_COLOR);
                drawStars();

                char overtext[50] = "CAPTALE'S SPACE SHOOTER!";
                Vector2 overtextSize = MeasureTextEx(font, overtext, FONT_SIZE, 0);
                DrawTextEx(font, overtext, {WINDOW_WIDTH/2 - overtextSize.x / 2, overtextSize.y / 2 + 100}, FONT_SIZE, 0, WHITE);

                sprintf(overtext, "-SELECT GAME MODE-");
                overtextSize = MeasureTextEx(font, overtext, FONT_SIZE/2, 0);
                DrawTextEx(font, overtext, {WINDOW_WIDTH/2 - overtextSize.x / 2,WINDOW_HEIGHT/2 - overtextSize.y / 2 - 60}, FONT_SIZE/2, 0, WHITE);

                sprintf(overtext, "PRESS 1: 100 SECONDS - PRESS 2: 60 LASERS");
                overtextSize = MeasureTextEx(font, overtext, FONT_SIZE/3, 0);
                DrawTextEx(font, overtext, {WINDOW_WIDTH/2 - overtextSize.x / 2,WINDOW_HEIGHT/2 - overtextSize.y / 2 + 20}, FONT_SIZE/3, 0, WHITE);

                sprintf(overtext, "PRESS 3: FREESTYLE (PowerUps Disabled)");
                overtextSize = MeasureTextEx(font, overtext, FONT_SIZE/3, 0);
                DrawTextEx(font, overtext, {WINDOW_WIDTH/2 - overtextSize.x / 2,WINDOW_HEIGHT/2 - overtextSize.y / 2 + 90}, FONT_SIZE/3, 0, WHITE);

                
                if(IsKeyPressed(KEY_ONE)) {
                    selectedGameMode = HUNDREDSECONDS;
                } else if(IsKeyPressed(KEY_TWO)) {
                    selectedGameMode = SIXTYLASERS;
                } else if(IsKeyPressed(KEY_THREE)) {
                    selectedGameMode = FREESTYLE_NOPOWERUPS;
                }

                if(selectedGameMode != -1) {
                    if(selectedGameMode == HUNDREDSECONDS) {
                        sprintf(overtext, "Current Selection: 100 Seconds");
                    } else if(selectedGameMode == SIXTYLASERS) {
                        sprintf(overtext, "Current Selection: 60 Lasers");
                    } else {
                        sprintf(overtext, "Current Selection: FreeStyle");
                    }
                    overtextSize = MeasureTextEx(font, overtext, FONT_SIZE/4, 0);
                    DrawTextEx(font, overtext, {WINDOW_WIDTH/2 - overtextSize.x / 2,WINDOW_HEIGHT - overtextSize.y/2 - 140}, FONT_SIZE/4, 0, WHITE);
                }

                sprintf(overtext, "Press RIGHTSHIFT to confirm selection");
                overtextSize = MeasureTextEx(font, overtext, FONT_SIZE/4, 0);
                DrawTextEx(font, overtext, {WINDOW_WIDTH/2 - overtextSize.x / 2,WINDOW_HEIGHT - overtextSize.y/2 - 100}, FONT_SIZE/4, 0, WHITE);

                sprintf(overtext, "Press L to return to Lobby");
                overtextSize = MeasureTextEx(font, overtext, FONT_SIZE/4, 0);
                DrawTextEx(font, overtext, {WINDOW_WIDTH/2 - overtextSize.x / 2,WINDOW_HEIGHT - overtextSize.y/2 - 60}, FONT_SIZE/4, 0, WHITE);
                
                if(IsKeyPressed(KEY_RIGHT_SHIFT)) {
                    selected = true;
                    if(selectedGameMode == HUNDREDSECONDS) {
                        overTimer = Timer(MAX_TIME, false, true, [this]() {updateGameOver();});
                    } 
                    if(selectedGameMode!= FREESTYLE_NOPOWERUPS) {
                        powerUpTimer = Timer(POWERUP_TIMER_DURATION, true, true, [this]() { createPowerUp(); });
                    }
                }

                if(IsKeyPressed(KEY_L)) {
                    returnToLobby = true;
                }
            } else if(!gameOver) {
                ClearBackground(BG_COLOR);
                drawStars();
                Vector2 pressSize = MeasureTextEx(font, "Press L to return to Lobby", FONT_SIZE/4, 0);
                DrawTextEx(font, "Press L to return to Lobby", {WINDOW_WIDTH/2 - pressSize.x/2, 50}, FONT_SIZE/4, 0, WHITE);
                player->draw();
                drawScore();
                for(int i = 0; i < numLasers; i++) {
                    lasers[i]->draw();
                }
                for(int i = 0; i < numMeteors; i++) {
                    meteors[i]->draw();
                }
                for(int i = 0; i < numExplosions; i++) {
                    explosions[i]->draw();
                }
                if(selectedGameMode != FREESTYLE_NOPOWERUPS) {
                    for(int i = 0; i < numPowerUps; i++) {
                        powerUps[i]->draw();
                    }
                }

                if(selectedGameMode == SIXTYLASERS) {
                    string text = "Lasers Used: " + to_string(lasersUsed) + "/60";
                    Vector2 textSize = MeasureTextEx(font, text.c_str(), FONT_SIZE/4, 0);
                    DrawTextEx(font, text.c_str(), {150 - textSize.x / 2, textSize.y / 2 + 120}, FONT_SIZE/4, 0, WHITE);
                }

                if(selectedGameMode == HUNDREDSECONDS) {
                    timeSurvived = overTimer.getTimePassed();
                    string text = "Time Passed: " + to_string(int(timeSurvived));
                    Vector2 textSize = MeasureTextEx(font, text.c_str(), FONT_SIZE/4, 0);
                    DrawTextEx(font, text.c_str(), {WINDOW_WIDTH - 150 - textSize.x / 2, textSize.y / 2 + 120}, FONT_SIZE/4, 0, WHITE);
                }

                if(player->getTripleShotStatus()) {
                    string text = "Triple Shot: Active";
                    Vector2 textSize = MeasureTextEx(font, text.c_str(), FONT_SIZE/4, 0);
                    DrawTextEx(font, text.c_str(), {150 - textSize.x / 2, WINDOW_HEIGHT - textSize.y / 2 - 120}, FONT_SIZE/4, 0, {0, 166, 125, 255});
                }

                if(player->getShieldStatus()) {
                    string text = "Shield: Active";
                    Vector2 textSize = MeasureTextEx(font, text.c_str(), FONT_SIZE/4, 0);
                    DrawTextEx(font, text.c_str(), {WINDOW_WIDTH - 150 - textSize.x / 2, WINDOW_HEIGHT - textSize.y / 2 - 120}, FONT_SIZE/4, 0, {0, 166, 125, 255});
                }
                if(IsKeyPressed(KEY_L)) {
                    returnToLobby = true;
                }
            } else if(gameOver) {

                ClearBackground(BG_COLOR);
            
                drawStars();
                drawStars();
                drawStars();
            
                char overtext[100];
                Vector2 overtextSize;
            
                sprintf(overtext, "GAME OVER!");
                overtextSize = MeasureTextEx(font, overtext, FONT_SIZE, 0);
                DrawTextEx(font, overtext, {WINDOW_WIDTH / 2 - overtextSize.x / 2, overtextSize.y/2 + 100}, FONT_SIZE, 0, WHITE);
            
                int y = 0;
                if(selectedGameMode == HUNDREDSECONDS) {
                    y = -100;
                }
                
                sprintf(overtext, "Score: %d", score);
                overtextSize = MeasureTextEx(font, overtext, FONT_SIZE / 2, 0);
                DrawTextEx(font, overtext, {WINDOW_WIDTH / 2 - overtextSize.x / 2, WINDOW_HEIGHT/2 - overtextSize.y/2 + y}, FONT_SIZE / 2, 0, WHITE);
            
                if (selectedGameMode == HUNDREDSECONDS) {
                    double survived = timeSurvived;
                    sprintf(overtext, "Time Survived: %.1f / 100 sec", survived);
                    overtextSize = MeasureTextEx(font, overtext, FONT_SIZE / 3, 0);
                    DrawTextEx(font, overtext, {WINDOW_WIDTH / 2 - overtextSize.x / 2, WINDOW_HEIGHT/2 - overtextSize.y/2}, FONT_SIZE / 3, 0, WHITE);
                }
            
                if (selectedGameMode != FREESTYLE_NOPOWERUPS) {
                    float accuracy = (lasersUsed > 0) ? (score * 100.0f / lasersUsed) : 0.0f;
                    sprintf(overtext, "Accuracy: %.1f%% (%d/%d)", accuracy, score, lasersUsed);
                    overtextSize = MeasureTextEx(font, overtext, FONT_SIZE / 3, 0);
                    DrawTextEx(font, overtext, {WINDOW_WIDTH / 2 - overtextSize.x / 2, WINDOW_HEIGHT/2 - overtextSize.y/2 + 60}, FONT_SIZE / 3, 0, WHITE);
            
                    sprintf(overtext, "TripleShots Used: %d", tripleShotsCounter);
                    overtextSize = MeasureTextEx(font, overtext, FONT_SIZE / 3, 0);
                    DrawTextEx(font, overtext, {WINDOW_WIDTH / 2 - overtextSize.x / 2, WINDOW_HEIGHT/2 - overtextSize.y/2 + 120}, FONT_SIZE / 3, 0, WHITE);
            
                    sprintf(overtext, "Shields Used: %d", shieldCounter);
                    overtextSize = MeasureTextEx(font, overtext, FONT_SIZE / 3, 0);
                    DrawTextEx(font, overtext, {WINDOW_WIDTH / 2 - overtextSize.x / 2, WINDOW_HEIGHT/2 - overtextSize.y/2 + 180}, FONT_SIZE / 3, 0, WHITE);
                }
            
                sprintf(overtext, "Thank you for playing!");
                overtextSize = MeasureTextEx(font, overtext, FONT_SIZE / 4, 0);
                DrawTextEx(font, overtext, {WINDOW_WIDTH / 2 - overtextSize.x / 2, WINDOW_HEIGHT - 160 - overtextSize.y/2}, FONT_SIZE / 4, 0, WHITE);
            
                sprintf(overtext, "Press L to Exit");
                overtextSize = MeasureTextEx(font, overtext, FONT_SIZE / 4, 0);
                DrawTextEx(font, overtext, {WINDOW_WIDTH / 2 - overtextSize.x / 2, WINDOW_HEIGHT - 100 - overtextSize.y/2}, FONT_SIZE / 4, 0, WHITE);
            
                if (IsKeyPressed(KEY_L)) {
                    returnToLobby = true;
                }

            }

            

        }

        void discardSprites() {
            int tempNumLasers = numLasers;
            for(int i = 0; i < numLasers; i++) {
                if(lasers[i]->getDiscardStatus()) {
                    tempNumLasers--;
                }
            }
            Laser** tempLasers = new Laser*[tempNumLasers];
            int lasersAdded = 0;
            for(int i = 0; i < numLasers; i++) {
                if(!lasers[i]->getDiscardStatus()) {
                    tempLasers[lasersAdded] = lasers[i];
                    lasersAdded++;
                } else {
                    delete lasers[i];
                }
            }
            delete[] lasers;
            lasers = tempLasers;
            numLasers = lasersAdded;

            int tempNumMeteors = numMeteors;
            for(int i = 0; i < numMeteors; i++) {
                if(meteors[i]->getDiscardStatus()) {
                    tempNumMeteors--;
                }
            }
            Meteor** tempMeteors = new Meteor*[tempNumMeteors];
            int meteorsAdded = 0;
            for(int i = 0; i < numMeteors; i++) {
                if(!meteors[i]->getDiscardStatus()) {
                    tempMeteors[meteorsAdded] = meteors[i];
                    meteorsAdded++;
                } else {
                    delete meteors[i];
                }
            }
            delete[] meteors;
            meteors = tempMeteors;
            numMeteors = meteorsAdded;


            int tempNumExplosions = numExplosions;
            for(int i = 0; i < numExplosions; i++) {
                if(explosions[i]->getDiscardStatus()) {
                    tempNumExplosions--;
                }
            }
            ExplosionAnimation** tempExplosions = new ExplosionAnimation*[tempNumExplosions];
            int explosionsAdded = 0;
            for(int i = 0; i < numExplosions; i++) {
                if(!explosions[i]->getDiscardStatus()) {
                    tempExplosions[explosionsAdded] = explosions[i];
                    explosionsAdded++;
                } else {
                    delete explosions[i];
                }
            }
            delete[] explosions;
            explosions = tempExplosions;
            numExplosions = explosionsAdded;

            int tempNumPowerUps = numPowerUps;
            for(int i = 0; i < numPowerUps; i++) {
                if(powerUps[i]->getDiscardStatus()) {
                    tempNumPowerUps--;
                }
            }
            PoweredUp** tempPowerUps = new PoweredUp*[tempNumPowerUps];
            int powerUpsAdded = 0;
            for(int i = 0; i < numPowerUps; i++) {
                if(!powerUps[i]->getDiscardStatus()) {
                    tempPowerUps[powerUpsAdded] = powerUps[i];
                    powerUpsAdded++;
                } else {
                    delete powerUps[i];
                }
            }
            delete[] powerUps;
            powerUps = tempPowerUps;
            numPowerUps = powerUpsAdded;
        }

        void drawScore() {
            char text[20];
            sprintf(text, "%d", score);
            Vector2 textSize = MeasureTextEx(font, text, FONT_SIZE, 0);
            DrawTextEx(font, text, {WINDOW_WIDTH/2 - textSize.x / 2,100}, FONT_SIZE, 0, WHITE);
        }

        //GameOver Update
        void updateGameOver() {
            gameOver = true;
        }

        // GAME BEGIN
        bool run() {
            while (!WindowShouldClose()) {
                if (!selected) {
                    draw();
                }
                else if (!gameOver) {
                    update();
                    draw();
                }
                else {
                    draw();
                }
            }
            CloseAudioDevice();
            return false;
        }

        void restart() {
            for(int i = 0; i < numLasers; i++) {
                lasers[i]->updateDiscardStatus(true);
            }
            for(int i = 0; i < numMeteors; i++) {
                meteors[i]->updateDiscardStatus(true);
            }
            for(int i = 0; i < numPowerUps; i++) {
                powerUps[i]->updateDiscardStatus(true);
            }
            score = 0;
            lasersUsed = 0;
            numMeteors = 0;
            numLasers = 0;
            numPowerUps = 0;
            player->disablePowerUps();
            selected = false;
            gameOver = false;
        }

        // FREE ALLOCATED MEMORY
        ~SpaceShooter() {
            for (int i = 0; i < numLasers; i++) {
                delete lasers[i];
            }
            delete[] lasers;
            for(int i = 0; i < numMeteors; i++) {
                delete meteors[i];
            }
            delete[] meteors;
            for(int i = 0; i < numExplosions; i++) {
                delete explosions[i];
            }
            delete[] explosions;
            for(int i = 0; i < numPowerUps; i++) {
                delete powerUps[i];
            }
            delete[] powerUps;
            CloseAudioDevice();
        }

};


#endif
