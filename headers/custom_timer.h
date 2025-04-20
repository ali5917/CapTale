#include "C:\raylib\raylib\src\raylib.h"
#include <iostream>
#include <cstdlib>
#include <ctime> 
#include <functional>
#include "settings.h"
using namespace std;

class Timer {
    private:
        float duration;
        float startTime;
        bool active;
        bool repeat;
        function<void()> func;
    public:
        Timer(float d, bool r=false, bool autoStart=false, function<void()> f=nullptr) :duration(d), startTime(0), active(false), repeat(r), func(f) {
            if(autoStart) {
                activate();
            }
        }
        void activate() {
            active = true;
            startTime = GetTime();
        }
        void deactivate() {
            active = false;
            startTime = 0;
            if(repeat) {
                activate();
            }
        }
        void update() {
            if(active) {
                if(GetTime() - startTime >= duration) {
                    if(func && startTime) {
                        func();
                    }
                    deactivate();
                }
            }
        }
        double getTimePassed() {
            return GetTime() - startTime;
        }
};