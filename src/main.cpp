#include <fstream>
#include <iostream>
#include <vector>

#include "torch/torch.h"

#include "structs.hpp"

#include <time.h>

void get_bullets(std::vector<bullet>& bullets)
{
    bullets.clear();

    std::ifstream inp;
    inp.open("bin.dat", std::ios::in | std::ios::binary);

    bullet b;
    while (!inp.fail())
    {
        bullets.push_back(b);
        inp.read((char*)&b, sizeof(float) * 4);
    }
    bullets.erase(bullets.begin());
}

int main()
{
    std::vector<bullet> bullets;
    clock_t tStart;

    while (true)
    {
        tStart = clock();
        get_bullets(bullets);


        printf("Time taken: %s\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
    }

    return 0;
}
