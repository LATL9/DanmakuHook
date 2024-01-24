#include <iostream>

#include <torch/torch.h>

int main()
{
    std::cout << "Hello, world!";
    torch::Tensor x = torch::Tensor(3, 3);
    return 0;
}
