#ifndef CONTROLS_H
#define CONTROLS_H

#include <array>
#include <time.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

#include "common.hpp"

class controls
{
    public:
        controls();
        std::array<unsigned int, 4> get_keys();
        void exec_action(std::array<std::array<unsigned int, 4>, FRAMES_PER_ACTION> output, clock_t time, std::array<unsigned int, 4> keys);

    private:
        Display* display;
};

#endif
