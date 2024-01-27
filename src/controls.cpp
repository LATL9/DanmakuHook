#include <iostream>
#include "controls.hpp"

controls::controls()
{
    Window rootwindow;

    display = XOpenDisplay(NULL);
    rootwindow = DefaultRootWindow(display);

    XSelectInput(display, rootwindow, KeyPressMask);

    // hold Z key to shoot
    XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Z), 1, 0);
}

// have to create KEYS array here to prevent collision w/ libtorch and X11 header files
std::array<unsigned int, 4> controls::get_keys()
{
    return std::array<unsigned int, 4> {
        XKeysymToKeycode(display, XK_Up),
        XKeysymToKeycode(display, XK_Down),
        XKeysymToKeycode(display, XK_Left),
        XKeysymToKeycode(display, XK_Right)
    };
}

void controls::exec_action(std::array<std::array<unsigned int, 4>, FRAMES_PER_ACTION> output, clock_t time, std::array<unsigned int, 4> keys)
{
    for (size_t i = 0; i < FRAMES_PER_ACTION; ++i)
    {
            // sync each action to clock
            while ((double)(clock() - time) / CLOCKS_PER_SEC < FRAME_TIME + ACTION_TIME * (i + 1)) { continue; }
            for (size_t j = 0; j < 4; ++j)
            {
                XTestFakeKeyEvent(display, keys[j], output[i][j], 0);
            }
    }
}

