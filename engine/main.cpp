#include <iostream>
#include <string>

int main()
{
    std::cout << "Browser engine initialized!" << std::endl;

    std::string message;
    while (std::getline(std::cin, message))
    {
        if (message == "exit")
        {
            break;
        }
        std::cout << "Engine received: " << message << std::endl;
    }

    return 0;
}
