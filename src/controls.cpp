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
    output = std::array<std::array<unsigned int, 4>, 3>{
        std::array<unsigned int, 4>{ 1, 0, 0, 0 },
        std::array<unsigned int, 4>{ 0, 1, 0, 0 },
        std::array<unsigned int, 4>{ 0, 0, 1, 0 },
    };

    for (size_t i = 0; i < 4; ++i)
    {
        if (output[0][i])
        {
            std::cout << "LKJASDLKDA1111111111111\n";
            switch (i)
            {
                case 0:
                    std::cout << "↑";
                    break;

                case 1:
                    std::cout << "↓";
                    break;

                case 2:
                    std::cout << "←";
                    break;

                case 3:
                    std::cout << "→";
                    break;
            }
            XTestFakeKeyEvent(display, keys[i], 1, 0);
        }
    }
    // sync each action to clock
    for (size_t i = 1; i < FRAMES_PER_ACTION; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            if (output[i][j] && !output[i - 1][j])
            {
                std::cout << "LKJASDLKDA\n";
                switch (j)
                {
                    case 0:
                        std::cout << "↑";
                        break;

                    case 1:
                        std::cout << "↓";
                        break;

                    case 2:
                        std::cout << "←";
                        break;

                    case 3:
                        std::cout << "→";
                        break;
                }
                XTestFakeKeyEvent(display, keys[j], 1, 0);
            }
            if (!output[i][j] && output[i - 1][j])
            {
                std::cout << "LKJASDLKDA22222222222222222\n";
                XTestFakeKeyEvent(display, keys[j], 0, 0);
            }
        }
        XFlush(display);
        // sync each action to clock
        while ((double)(clock() - time) / CLOCKS_PER_SEC < FRAME_TIME + ACTION_TIME * (i + 1)) { continue; }
    }
    // release all keys
    for (size_t i = 0; i < 4; ++i)
    {
        XTestFakeKeyEvent(display, keys[i], 0, 0);
    }
    XFlush(display);
}

