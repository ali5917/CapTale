#include <iostream>
#include "C:\raylib\raylib\src\raylib.h"
#include <fstream>
using namespace std;

// Global Variables/Constants

const int screenWidth = 1920;
const int screenHeight = 1080;






int main () {

    InitWindow(screenWidth, screenHeight, "CapTale");

    SetTargetFPS(60);
    while (WindowShouldClose() == false) {
        
        
        

        // Drawing
        BeginDrawing();

        ClearBackground(BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}