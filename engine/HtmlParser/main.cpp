#include "Tokenizer.h"
#include <iostream>

int main()
{
    std::string html = R"(<div class="container" id="main">Hello, World!</div>)";
    Tokenizer tokenizer(html);

    auto tokens = tokenizer.tokenize();
    for (const auto &token : tokens)
    {
        std::cout << "Token Type: " << static_cast<int>(token->type)
                  << ", Data: " << token->data << std::endl;
        for (const auto &attr : token->attributes)
        {
            std::cout << "  Attribute: " << attr.first << " = " << attr.second << std::endl;
        }
    }

    return 0;
}
