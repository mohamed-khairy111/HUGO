#include "Tokenizer.h"
#include <cctype>
#include <iostream>
#include <sstream>

std::unordered_map<std::string, char> Tokenizer::entityMap = {
    {"amp", '&'},
    {"lt", '<'},
    {"gt", '>'},
    {"quot", '"'},
    {"apos", '\''}};

std::string Tokenizer::decodeEntities(const std::string &input)
{
    std::string result;
    size_t i = 0;

    while (i < input.size())
    {
        if (input[i] == '&')
        {
            size_t semicolonPos = input.find(';', i);
            if (semicolonPos != std::string::npos)
            {
                std::string entity = input.substr(i + 1, semicolonPos - i - 1);

                // Named entity
                if (entityMap.find(entity) != entityMap.end())
                {
                    result += entityMap[entity];
                }
                // Numeric entity
                else if (entity[0] == '#')
                {
                    try
                    {
                        char decodeChar = static_cast<char>(std::stoi(entity.substr(1)));
                        result += decodeChar;
                    }
                    catch (...)
                    {
                        result += "&" + entity + ";"; // fallback for malformed entities
                    }
                }
                else
                {
                    result += "&" + entity + ";"; // fallback for unknown entities
                }

                i = semicolonPos + 1;
            }
            else
            {
                result += input[i++];
            }
        }
        else
        {
            result += input[i++];
        }
    }
    return result;
}

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
                    emitToken(tokens, TokenType::Text, decodeEntities(currentTokenData));
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
                auto token = std::make_shared<Token>(TokenType::StartTag, currentTokenData);
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
                std::string value;
                while ((currentChar = consumeCharacter()) != quote)
                {
                    value += currentChar;
                }
                value = decodeEntities(value);
                currentToken->attributes.emplace_back(currentAttributeName, value);
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
