#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <time.h>
#include <vector>

#include "torch/script.h"

#include "common.hpp"
#include "controls.hpp"
#include "structs.hpp"

void load_model(torch::jit::script::Module& model)
{
    model = torch::jit::load("model.pt");
}

void get_data(player& p, std::vector<bullet>& bullets)
{
    bullets.clear();

    std::ifstream inp;
    inp.open("input.dat", std::ios::in | std::ios::binary);

    // get player data
    if (!inp.fail())
    {
        inp.read((char*)&p, sizeof(vec2));
    }

    // get bullet data
    bullet b;
    while (!inp.fail())
    {
        bullets.push_back(b);
        inp.read((char*)&b, sizeof(vec2) * 2);
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
                        //input[0][index][std::max(std::min((int)(y + y_2), INPUT_SIZE - 1), 0)][std::max(std::min((int)(x + x_2), INPUT_SIZE - 1), 0)] = 1;
                    }
                }
            }
        }
    }
}

void get_action(torch::jit::script::Module model, torch::Tensor input, std::array<std::array<unsigned int, 4>, FRAMES_PER_ACTION> output)
{
    std::vector<torch::jit::IValue> inp = { input };
    at::Tensor y = model.forward(inp).toTensor();
    float* y_array = y.data_ptr<float>();

    for (size_t i = 0; i < FRAMES_PER_ACTION; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            if (y_array[i * 4 + j] > ACTION_THRESHOLD) { output[i][j] = 1; }
        }
        for (size_t j = 0; j < 4; j += 2)
        {
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
    }
}

int main()
{
    controls ctrls;
    const std::array<unsigned int, 4> KEYS = ctrls.get_keys();

    torch::jit::script::Module model; 
    torch::Tensor input = torch::empty({ 1, 2, 32, 32 }, torch::kFloat);
    std::vector<bullet> bullets;
    player p = { }; 

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
