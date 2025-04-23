#ifndef MESSAGE_H
#define MESSAGE_H

#include "C:\raylib\raylib\src\raylib.h"
#include "custom_timer.h"
#include <iostream>
using namespace std;

class Message {

    private:
        string text;
        Vector2 pos;
        int size;
        int duration;
        Timer message_timer;

};


#endif
