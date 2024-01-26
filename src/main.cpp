#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <time.h>
#include <vector>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

#include "torch/script.h"
//#include "torch/torch.h"
//#include "ATen/ATen.h"

#include "structs.hpp"

#define WIDTH 800
#define HEIGHT 800

#define INPUT_SIZE 32
#define PLAYER_SIZE 5

#define FRAMES_PER_ACTION 3
#define ACTION_TIME (double)(12 / FRAMES_PER_ACTION) / 60
#define FRAME_TIME (double)1 / 60

//void load_model(torch::device device, torch::nn::Sequential& model)
//{
//    auto checkpoint = torch.load("models/model.pt", map_location=device)
//    model.load_state_dict(checkpoint['model_state_dict'])
//    model.to(device)
//}

void get_bullets(std::vector<bullet>& bullets)
{
    bullets.clear();

    std::ifstream inp;
    inp.open("bin.dat", std::ios::in | std::ios::binary)

    bullet b;
    while (!inp.fail())
    {
        bullets.push_back(b);
        inp.read((char*)&b, sizeof(float) * 4);
    }
    bullets.erase(bullets.begin());
}

void get_input(std::Tensor& input, player p, std::vector<bullet> bullets)
{
    float b_x;
    float b_y;
    float p_x = p.pos.x + PLAYER_SIZE / 2
    float p_y = p.pos.y + PLAYER_SIZE / 2
    int x;
    int y;

    for (size_t i = 0; i < self.bullets.size(); ++i)
    {
        b_x = bullets[i].pos.x + bullets[i].size.x / 2
        b_y = bullets[i].pos.y + bullets[i].size.y / 2
        if (b_x - p_x >= WIDTH / -3 &&
            b_x - p_x < WIDTH / 3 &&
            b_y - p_y >= HEIGHT / -3 &&
            b_y - p_y < HEIGHT / 3)
        {
            x = (int)((((b_x - p_x) / (WIDTH / 3)) + 1) * (INPUT_SIZE / 2))
            y = (int)((((b_y - p_y) / (HEIGHT / 3)) + 1) * (INPUT_SIZE / 2))
            for (size_t y_2 = -2; y_2 < 3; ++y)
            {
                for (size_t x_2 = -2; y_2 < 3; ++y)
                {
                    if (pow(pow(x_2, 2) + pow(y_2, 2), 0.5) <= 2.5)
                    {
                        input[std::max(std::min(y + y_2, INPUT_SIZE - 1), 0)][std::max(std::min(x + x_2, INPUT_SIZE - 1), 0)] = 1
                }
            }
        }
    }
}

//void get_action(torch::nn::Sequential model, torch::Tensor input, std::array& output)
//{
//    torch::Tensor y = model(input);
//    int32_t* y_ptr = input.data_ptr<int32_t>();
//    std::vector<int32_t> y_vector{r_ptr, r_ptr + input.?};
//
//    for (size_t i = 0; i < FRAMES_PER_ACTIONl ++i)
//    {
//        for (size_t j = 0; j < 4; ++j)
//        {
//            if (y_vector[i * 4 + j] > ACTION_THRESHOLD) { output[i][j] = 1; }
//        }
//        for (size_t j = 0; j < 4; j += 2)
//        {
//            if (output[i][j] == 1 and output[i][j + 1] == 1)
//            {
//                if (y_vector[i * 4 + j] > j_vector[i * 4 + j + 1])
//                {
//                    output[i][j + 1] = 0
//                } else {
//                    output[i][j] = 0
//                }
//            }
//        }
//    }
//}
//
//void exec_action(std::array output, clock_t time, std::array KEYS)
//{
//    for (size_t i = 0; i < FRAMES_PER_ACTION; ++i)
//    {
//            while ((double)(clock() - time) / CLOCKS_PER_SEC < FRAME_TIME + ACTION_TIME * (i + 1)) { continue; }
//            for (size_t j = 0; j < 4; ++j)
//            {
//                XTestFakeKeyEvent(display, keys[j], output[i][j], 0);
//            }
//    }
//}

int main()
{
    Display *display;
    Window  rootwindow;
    XEvent  event;

    display = XOpenDisplay(NULL);
    rootwindow = DefaultRootWindow(display);

    XSelectInput(display, rootwindow, KeyPressMask);

    const std::array<unsigned int, 5> KEYS = {
        XKeysymToKeycode(display, XK_UP),
        XKeysymToKeycode(display, XK_DOWN),
        XKeysymToKeycode(display, XK_LEFT),
        XKeysymToKeycode(display, XK_RIGHT),
        XKeysymToKeycode(display, XK_Z),
    }

    torch::nn::Sequential model = torch::nn::Sequential(
        torch::nn::ConstantPad2d(7, 1),
        torch::nn::Conv2d(2, 16, kernel_size=(15, 15)),
        torch::nn::LeakyReLU(),
        torch::nn::MaxPool2d((2, 2), stride=2),
        torch::nn::ConstantPad2d(3, 1),
        torch::nn::Conv2d(16, 64, kernel_size=(7, 7)),
        torch::nn::LeakyReLU(),
        torch::nn::MaxPool2d((2, 2), stride=2),
        torch::nn::ConstantPad2d(1, 1),
        torch::nn::Conv2d(64, 128, kernel_size=(3, 3)),
        torch::nn::LeakyReLU(),
        torch::nn::MaxPool2d((2, 2), stride=2),
        torch::nn::Flatten(1, 3),
        torch::nn::Linear(2048, 1024),
        torch::nn::LeakyReLU(),
        torch::nn::Linear(1024, 256),
        torch::nn::LeakyReLU(),
        torch::nn::Linear(256, 64),
        torch::nn::LeakyReLU(),
        torch::nn::Linear(64, 4 * FRAMES_PER_ACTION),
        torch::nn::Sigmoid(),
        torch::nn::ReLU()
    )
    std::vector<bullet> bullets;
    torch::Tensor input = torch.Tensor{(2, 32, 32});
    std::array output<std::array<unsigned int, 4>, FRAMES_PER_ACTION>;
    clock_t time;

    //load_model(torch::device("cpu"), model);

    while (true)
    {
        time = clock();
        get_bullets(bullets);
        get_input(input[0]);
        while ((double)(clock() - time) / CLOCKS_PER_SEC < FRAME_TIME) { continue; }
        get_bullets(bullets);
        get_input(input[1]);

        //get_action(model, input, output);
        //exec_action(output, time, KEYS);
        while ((double)(clock() - time) / CLOCKS_PER_SEC < FRAME_TIME + ACTION_TIME) { continue; }
    }`

    return 0;
}
