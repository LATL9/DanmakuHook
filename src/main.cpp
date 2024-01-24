#include <fstream>
#include <iostream>
#include <vector>

#include "structs.hpp"

int main()
{
    return 0;
}

void on_tick(std::vector<bullet>* bullets)
{
    bullets->clear();

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

    for (size_t i = 0; i < length; ++i)
    {
        inp.read((void*)&b, 16);
        bullets->push_back(b);
    }
}

int main()
{
    std::vector<bullet> bullets;
    on_tick(bullets);
    return 0;
}
