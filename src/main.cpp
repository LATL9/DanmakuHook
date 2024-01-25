#include <fstream>
#include <iostream>
#include <vector>

#include "torch/torch.h"

#include "structs.hpp"

#include <time.h>

void on_tick(std::vector<bullet>& bullets)
{
    bullets.clear();

    std::ifstream inp;
    inp.open("bin.dat", std::ios::in | std::ios::binary);

    char s[4];
    inp.seekg (0, inp.end);
    size_t length = inp.tellg() / (sizeof(float) * 4);
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
    clock_t tStart = clock();
    on_tick(bullets);
    printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

    return 0;
}
