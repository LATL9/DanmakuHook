#include <iostream>
#include "controls.hpp"

controls::controls()
{
    // code required to send key events through X11
    Window rootwindow;

    display = XOpenDisplay(NULL); // NULL => display 0
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
    for (size_t i = 0; i < 4; ++i)
    {
        if (output[0][i])
        {
            // presses key
            XTestFakeKeyEvent(display, keys[i], 1, 0);
        }
    }
    // sync each action to clock
    for (size_t i = 1; i < FRAMES_PER_ACTION; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            // presses key if key is down now but was previously up
            if (output[i][j] && !output[i - 1][j])
            {
                XTestFakeKeyEvent(display, keys[j], 1, 0);
            }
            // releases key if key is up now but was previously down
            if (!output[i][j] && output[i - 1][j])
            {
                XTestFakeKeyEvent(display, keys[j], 0, 0);
            }
        }

        XFlush(display); // flush key events
        while ((double)(clock() - time) / CLOCKS_PER_SEC < FRAME_TIME + ACTION_TIME * (i + 1)) { continue; } // wait for end of action (sync with clock)
    }

    // release all keys
    for (size_t i = 0; i < 4; ++i)
    {
        XTestFakeKeyEvent(display, keys[i], 0, 0);
    }

    XFlush(display);
}

