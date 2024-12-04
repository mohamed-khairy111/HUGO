#include "../tokenizer/HTML5Tokenizer.h"
#include <iostream>

int main()
{
    // Test HTML input
    std::string html = R"(<!DOCTYPE html><html lang="en"><!-- This is a comment --><body class="main">Hello, world!</body>)";

    // Initialize tokenizer
    HTML5Tokenizer tokenizer(html);

    // Tokenize input
    auto tokens = tokenizer.tokenize();

    // Print tokens
    for (const auto &token : tokens)
    {
        std::cout << "Token: " << token->toString() << std::endl;
    }

    return 0;
}
