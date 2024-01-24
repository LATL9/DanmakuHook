#include <iostream>

#include <torch/torch.h>

int main()
{
    std::cout << "Hello, world!";
    torch::Tensor x = torch::ones(9);
    return 0;
}
