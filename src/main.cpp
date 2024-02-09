#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <time.h>
#include <vector>

#include "torch/script.h"

#include "common.hpp"
#include "controls.hpp"

void load_model(torch::jit::script::Module& model)
{
    model = torch::jit::load("model.pt");
}

void get_data(player& p, std::vector<bullet>& bullets)
{
    // use temp file to prevent overwriting while reading file
    std::filesystem::copy("input.bin", "input.bin.tmp", std::filesystem::copy_options::overwrite_existing);
    bullets.clear();

    std::ifstream inp;
    inp.open("input.bin.tmp", std::ios::in | std::ios::binary);

    // get player data
    if (!inp.fail())
    {
        inp.read((char*)&p, sizeof(player));
    }

    // get bullet data
    bullet b;
    inp.read((char*)&b, sizeof(bullet));
    if (!inp.fail())
    {
        while (!inp.fail())
        {
            bullets.push_back(b);
            inp.read((char*)&b, sizeof(bullet));
        }
    }
}

bool get_input(torch::Tensor& input, size_t index, player& p, std::vector<bullet> bullets)
{
    // whether or not a bullet was on-screen
    bool no_bullets = true;
    int x;
    int y;

    for (size_t i = 0; i < bullets.size(); ++i)
    {
        // if bullet is on-screen (WIDTH / 3 x HEIGHT x 3 dimensions)
        if (bullets[i].pos.x - p.pos.x >= WIDTH / -3 &&
            bullets[i].pos.x - p.pos.x < WIDTH / 3 &&
            bullets[i].pos.y - p.pos.y >= HEIGHT / -3 &&
            bullets[i].pos.y - p.pos.y < HEIGHT / 3)
        {
            x = (int)((((bullets[i].pos.x - p.pos.x) / (WIDTH / 3)) + 1) * (INPUT_SIZE / 2));
            y = (int)((((bullets[i].pos.y - p.pos.y) / (HEIGHT / 3)) + 1) * (INPUT_SIZE / 2));

            // draw a circle around bullet's coordinates
            for (int y_2 = -2; y_2 < 3; ++y_2)
            {
                for (int x_2 = -2; x_2 < 3; ++x_2)
                {
                    // draw bullet like a circle (rounded edges)
                    if (pow(pow(x_2, 2) + pow(y_2, 2), 0.5) <= 1.5)
                    {
                        if (no_bullets)
                        {
                            no_bullets = false;
                        }
                        // make sure bullet is drawn inside 32 x 32 input tensor
                        input[0][index][std::max(std::min((int)(y + y_2), INPUT_SIZE - 1), 0)][std::max(std::min((int)(x + x_2), INPUT_SIZE - 1), 0)] = 1;
                    }
                }
            }
        }
    }

    return no_bullets;
}

void get_action(torch::jit::script::Module model, torch::Tensor input, std::array<std::array<unsigned int, 4>, FRAMES_PER_ACTION>& output)
{
    std::vector<torch::jit::IValue> inp = { input };
    at::Tensor y = model.forward(inp).toTensor();
    float* y_array = y.data_ptr<float>(); // action as a C-style array

    for (size_t i = 0; i < FRAMES_PER_ACTION; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            // considers pressing key if there is enough confidence
            if (y_array[i * 4 + j] > ACTION_THRESHOLD) { output[i][j] = 1; }
        }
        for (size_t j = 0; j < 4; j += 2)
        {
            // only press most confident key if two opposite keys (eg. ↑ and ↓) are being pressed 
            if (output[i][j] == 1 and output[i][j + 1] == 1)
            {
                if (y_array[i * 4 + j] > y_array[i * 4 + j + 1])
                {
                    output[i][j + 1] = 0;
                } else {
                    output[i][j] = 0;
                }
            }
        }

        // draw output in terminal
        for (size_t j = 0; j < 4; ++j)
        {
            switch (output[i][j])
            {
                case 0:
                    std::cout << " ";
                    break;

                default:
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
            }
        }
        std::cout << '\n';
    }
}

int main()
{
    controls ctrls; // encapsulates X11 libraries used to control player
    const std::array<unsigned int, 4> KEYS = ctrls.get_keys();

    torch::jit::script::Module model; 
    torch::Tensor input = torch::empty({ 1, 2, 32, 32 }, torch::kFloat);
    std::vector<bullet> bullets;
    player p = { }; 

    std::array<std::array<unsigned int, 4>, FRAMES_PER_ACTION> output;
    bool no_bullets;
    clock_t time;

    load_model(model);

    while (true)
    {
        input.fill_(0);
        output.fill(std::array<unsigned int, 4>{ 0 });
        time = clock();

        get_data(p, bullets);
        no_bullets = get_input(input, 0, p, bullets);
        while ((double)(clock() - time) / CLOCKS_PER_SEC < FRAME_TIME) { continue; } // wait for end of frame (sync w/ clock)

        get_data(p, bullets);
        if (!get_input(input, 1, p, bullets) or !no_bullets)
        {
            get_action(model, input, output);
            ctrls.exec_action(output, time, KEYS);
        }
        while ((double)(clock() - time) / CLOCKS_PER_SEC < FRAME_TIME + ACTION_TIME) { continue; } // wait for end of action (sync w/ clock)
    }

    return 0;
}
