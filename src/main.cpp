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
    std::filesystem::copy("input.bin", "input.bin.tmp", std::filesystem::copy_options::overwrite_existing);
    bullets.clear();

    std::ifstream inp;
    inp.open("input.bin.tmp", std::ios::in | std::ios::binary);

    // get player data
    if (!inp.fail())
    {
        inp.read((char*)&p, sizeof(vec2));
    }

    // get bullet data
    bullet b = { { 0, 0 }, { 0, 0 } };
    while (!inp.fail())
    {
        bullets.push_back(b);
        inp.read((char*)&b, sizeof(vec2) * 2);
    }
//    bullets.erase(bullets.begin());
}

void get_input(torch::Tensor& input, size_t index, player& p, std::vector<bullet> bullets)
{
    int x;
    int y;

    std::cout << "A\n";
    for (size_t i = 0; i < bullets.size(); ++i)
    {
        if (bullets[i].pos.x - p.pos.x >= WIDTH / -3 &&
            bullets[i].pos.x - p.pos.x < WIDTH / 3 &&
            bullets[i].pos.y - p.pos.y >= HEIGHT / -3 &&
            bullets[i].pos.y - p.pos.y < HEIGHT / 3)
        {
            x = (int)((((bullets[i].pos.x - p.pos.x) / (WIDTH / 3)) + 1) * (INPUT_SIZE / 2));
            y = (int)((((bullets[i].pos.y - p.pos.y) / (HEIGHT / 3)) + 1) * (INPUT_SIZE / 2));
            for (int y_2 = -2; y_2 < 3; ++y_2)
            {
                for (int x_2 = -2; x_2 < 3; ++x_2)
                {
                    if (pow(pow(x_2, 2) + pow(y_2, 2), 0.5) <= 2.5)
                    {
                        input[0][index][std::max(std::min((int)(y + y_2), INPUT_SIZE - 1), 0)][std::max(std::min((int)(x + x_2), INPUT_SIZE - 1), 0)] = 1;
                    }
                }
            }
        }
    }

    std::cout << "B\n";
    float* input_array = input.data_ptr<float>();
    for (int y_2 = 0; y_2 < INPUT_SIZE; ++y_2)
    {
        for (int x_2 = 0; x_2 < INPUT_SIZE; ++x_2)
        {
            switch ((int)input_array[y_2 * INPUT_SIZE + x_2])
            {
                case 0:
                    std::cout << "..";
                    break;

                default:
                    std::cout << "OO";
                    break;
            }
        }
        std::cout << '\n';
    }
    std::cout << "C\n";
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
                    break;
            }
        }
        std::cout << '\n';
    }
    std::cout << '\n';
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
        std::cout << "1\n";
        input.fill_(0);
        std::cout << "2\n";
        time = clock();

        std::cout << "3\n";
        get_data(p, bullets);
        std::cout << "4 length: " << bullets.size() << '\n';
        get_input(input, 0, p, bullets);
        std::cout << "5\n";
        while ((double)(clock() - time) / CLOCKS_PER_SEC < FRAME_TIME) { continue; }

        std::cout << "6\n";
        get_data(p, bullets);
        std::cout << "7 length: " << bullets.size() << '\n';
        get_input(input, 1, p, bullets);

        std::cout << "8\n";
        get_action(model, input, output);
        std::cout << "9\n";
        ctrls.exec_action(output, time, KEYS);
        std::cout << "10\n";
        while ((double)(clock() - time) / CLOCKS_PER_SEC < FRAME_TIME + ACTION_TIME) { continue; }
        std::cout << "11\n";
    }

    return 0;
}
