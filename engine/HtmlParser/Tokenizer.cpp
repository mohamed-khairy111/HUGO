#include "Tokenizer.h"
#include <cctype>
#include <iostream>

Tokenizer::Tokenizer(const std::string &input)
    : inputBuffer(input), currentPosition(0), state(State::Data) {}

std::vector<std::shared_ptr<Token>> Tokenizer::tokenize()
{
    std::vector<std::shared_ptr<Token>> tokens;

    auto currentToken = std::make_shared<Token>();

    while (currentPosition < inputBuffer.size())
    {
        char currentChar = consumeCharacter();

        switch (state)
        {
        case State::Data:
            if (currentChar == '<')
            {
                if (!currentTokenData.empty())
                {
                    emitToken(tokens, TokenType::Text, currentTokenData);
                    currentTokenData.clear();
                }
                switchState(State::TagOpen);
            }
            else
            {
                currentTokenData += currentChar;
            }
            break;

        case State::TagOpen:
            if (currentChar == '/')
            {
                switchState(State::TagClose);
            }
            else if (isalpha(currentChar))
            {
                switchState(State::TagName);
                currentTokenData += currentChar;
                currentToken = std::make_shared<Token>();
                currentToken->type = TokenType::StartTag;
                currentToken->data = currentChar;
            }
            else
            {
                std::cerr << "Unexpected character: " << currentChar << std::endl;
            }
            break;

        case State::TagName:
            if (isalnum(currentChar))
            {
                currentTokenData += currentChar;
                currentToken->data = currentTokenData;
            }
            else if (currentChar == '>')
            {
                tokens.push_back(currentToken);
                currentTokenData.clear();
                switchState(State::Data);
            }
            else if (currentChar == '/')
            {
                switchState(State::SelfClosingTag);
            }
            else if (isspace(currentChar))
            {
                switchState(State::AttributeName);
            }
            else
            {
                std::cerr << "Unexpected character in tag name: " << currentChar << std::endl;
            }
            break;

        case State::AttributeName:
            if (isalnum(currentChar) || currentChar == '-')
            {
                currentAttributeName += currentChar;
            }
            else if (currentChar == '=')
            {
                switchState(State::AttributeValue);
            }
            else if (isspace(currentChar))
            {
                // Allow spaces after attribute names
            }
            else if (currentChar == '>')
            {
                tokens.push_back(currentToken);
                currentTokenData.clear();
                switchState(State::Data);
            }
            break;

        case State::AttributeValue:
            if (currentChar == '"' || currentChar == '\'')
            {
                char quote = currentChar; // Track quote type
                currentAttributeValue.clear();
                while ((currentChar = consumeCharacter()) != quote)
                {
                    currentAttributeValue += currentChar;
                }
                currentToken->attributes.emplace_back(currentAttributeName, currentAttributeValue);
                currentAttributeName.clear();
                switchState(State::AttributeName);
            }
            else
            {
                std::cerr << "Expected quotes for attribute value!" << std::endl;
            }
            break;

        case State::TagClose:
            if (isalnum(currentChar))
            {
                currentTokenData += currentChar;
            }
            else if (currentChar == '>')
            {
                emitToken(tokens, TokenType::EndTag, currentTokenData);
                currentTokenData.clear();
                switchState(State::Data);
            }
            else
            {
                std::cerr << "Unexpected character in closing tag: " << currentChar << std::endl;
            }
            break;

        case State::SelfClosingTag:
            if (currentChar == '>')
            {
                tokens.push_back(currentToken);
                currentTokenData.clear();
                switchState(State::Data);
            }
            else
            {
                std::cerr << "Unexpected character in self-closing tag: " << currentChar << std::endl;
            }
            break;
        }
    }

    // Emit any remaining text token
    if (!currentTokenData.empty() && state == State::Data)
    {
        emitToken(tokens, TokenType::Text, currentTokenData);
    }

    return tokens;
}

void Tokenizer::switchState(State newState)
{
    state = newState;
}

char Tokenizer::consumeCharacter()
{
    return inputBuffer[currentPosition++];
}

void Tokenizer::emitToken(std::vector<std::shared_ptr<Token>> &tokens, TokenType type, const std::string &data)
{
    auto token = std::make_shared<Token>();
    token->type = type;
    token->data = data;

    tokens.push_back(token);

    std::cout << "Token emitted: Type = " << static_cast<int>(type) << ", Data = " << data << std::endl;
}
