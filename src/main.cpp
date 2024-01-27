#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <time.h>
#include <vector>

#include "torch/torch.h"

#include "common.hpp"
#include "controls.hpp"
#include "structs.hpp"

void load_model(torch::jit::script::Module& model)
{
    torch::jit::script::Module model;
    model = torch::jit::load("model.pt");
}

void get_data(player& p, std::vector<bullet>& bullets)
{
    bullets.clear();

    std::ifstream inp;
    inp.open("bin.dat", std::ios::in | std::ios::binary);

    // get player data
    if (!inp.fail())
    {
        inp.read((char*)&p, sizeof(float) * 2);
    }

    // get bullet data
    bullet b;
    while (!inp.fail())
    {
        bullets.push_back(b);
        inp.read((char*)&b, sizeof(float) * 4);
    }
    bullets.erase(bullets.begin());
}

void get_input(torch::Tensor& input, size_t index, player& p, std::vector<bullet> bullets)
{
    float b_x;
    float b_y;
    float p_x = p.pos.x + PLAYER_SIZE / 2;
    float p_y = p.pos.y + PLAYER_SIZE / 2;
    int x;
    int y;

    for (size_t i = 0; i < bullets.size(); ++i)
    {
        b_x = bullets[i].pos.x + bullets[i].size.x / 2;
        b_y = bullets[i].pos.y + bullets[i].size.y / 2;
        if (b_x - p_x >= WIDTH / -3 &&
            b_x - p_x < WIDTH / 3 &&
            b_y - p_y >= HEIGHT / -3 &&
            b_y - p_y < HEIGHT / 3)
        {
            x = (int)((((b_x - p_x) / (WIDTH / 3)) + 1) * (INPUT_SIZE / 2));
            y = (int)((((b_y - p_y) / (HEIGHT / 3)) + 1) * (INPUT_SIZE / 2));
            for (size_t y_2 = -2; y_2 < 3; ++y_2)
            {
                for (size_t x_2 = -2; x_2 < 3; ++x_2)
                {
                    if (pow(pow(x_2, 2) + pow(y_2, 2), 0.5) <= 2.5)
                    {
                        input[index][std::max(std::min((int)(y + y_2), INPUT_SIZE - 1), 0)][std::max(std::min((int)(x + x_2), INPUT_SIZE - 1), 0)] = 1;
                    }
                }
            }
        }
    }
}

void get_action(torch::nn::Sequential model, torch::Tensor input, std::array<std::array<unsigned int, 4>, FRAMES_PER_ACTION> output)
{
    torch::Tensor y = model(input);
    int32_t* y_ptr = input.data_ptr<int32_t>();
    std::vector<int32_t> y_vector{y_ptr, y_ptr + input.?};

    for (size_t i = 0; i < FRAMES_PER_ACTION; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            if (y_vector[i * 4 + j] > ACTION_THRESHOLD) { output[i][j] = 1; }
        }
        for (size_t j = 0; j < 4; j += 2)
        {
            if (output[i][j] == 1 and output[i][j + 1] == 1)
            {
                if (y_vector[i * 4 + j] > y_vector[i * 4 + j + 1])
                {
                    output[i][j + 1] = 0;
                } else {
                    output[i][j] = 0;
                }
            }
        }
    }
}

int main()
{
    controls ctrls;
    const std::array<unsigned int, 4> KEYS = ctrls.get_keys();

    torch::jit::script::Module& 
    torch::nn::Sequential model = torch::nn::Sequential(
        torch::nn::ConstantPad2d(torch::nn::ConstantPad2dOptions(7, 1)),
        torch::nn::Conv2d(torch::nn::Conv2dOptions(2, 16, 15)),
        torch::nn::LeakyReLU(),
        torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2)),
        torch::nn::ConstantPad2d(torch::nn::ConstantPad2dOptions(3, 1)),
        torch::nn::Conv2d(torch::nn::Conv2dOptions(16, 64, 7)),
        torch::nn::LeakyReLU(),
        torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2)),
        torch::nn::ConstantPad2d(torch::nn::ConstantPad2dOptions(1, 1)),
        torch::nn::Conv2d(torch::nn::Conv2dOptions(64, 128, 3)),
        torch::nn::LeakyReLU(),
        torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2)),
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
    );

    std::vector<bullet> bullets;
    player p = { }; 
    torch::Tensor input = torch::full({ 2, 32, 32 }, 0);

    std::array<std::array<unsigned int, 4>, FRAMES_PER_ACTION> output;
    clock_t time;

    load_model(model);

    while (true)
    {
        time = clock();
        get_data(p, bullets);
        get_input(input, 0, p, bullets);
        while ((double)(clock() - time) / CLOCKS_PER_SEC < FRAME_TIME) { continue; }
        get_data(p, bullets);
        get_input(input, 1, p, bullets);

        get_action(model, input, output);
        ctrls.exec_action(output, time, KEYS);
        while ((double)(clock() - time) / CLOCKS_PER_SEC < FRAME_TIME + ACTION_TIME) { continue; }
    }

    return 0;
}
