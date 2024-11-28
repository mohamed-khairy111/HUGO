#include "Tokenizer.h"
#include <iostream>

int main()
{
    std::string input = "<div>Hello, World!<br/></div>";
    Tokenizer tokenizer(input);

    auto tokens = tokenizer.tokenize();
    for (const auto &token : tokens)
    {
        std::cout << "Token Type: " << static_cast<int>(token->type)
                  << ", Data: " << token->data << std::endl;
    }

    return 0;
}
