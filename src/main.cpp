#include <fstream>
#include <iostream>
#include <vector>

#include "torch/torch.h"

#include "structs.hpp"

void on_tick(std::vector<bullet>& bullets)
{
    bullets.clear();

    float pos_x;
    float pox_y;
    float size_x;
    float size_y;

    std::ifstream inp;
    inp.open("bin.dat", std::ios::in | std::ios::binary);

    char s[4];
    inp.seekg (0, inp.end);
    size_t length = inp.tellg() / (4 * 4);
    inp.seekg (0, inp.beg);
    bullet b;

    while (!inp.fail())
    {
        bullets.push_back(b);
        inp.read((char*)&b, 16);
    }
    bullets.erase(bullets.begin());
}

int main()
{
    std::vector<bullet> bullets;
    on_tick(bullets);
    return 0;
}
