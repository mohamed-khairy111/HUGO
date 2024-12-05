#include "HTML5Tokenizer.h"
#include <cctype>
#include <iostream>

HTML5Tokenizer::HTML5Tokenizer(const std::string &html)
    : input(html), position(0), state(State::Data) {}

std::vector<std::shared_ptr<Token>> HTML5Tokenizer::tokenize()
{
    std::vector<std::shared_ptr<Token>> tokens;
    auto currentToken = std::make_shared<TagToken>(TokenType::StartTag, "");

    while (position < input.size())
    {
        char currentChar = consumeCharacter();

        switch (state)
        {
        case State::Data:
            if (currentChar == '<')
            {
                if (!currentData.empty())
                {
                    emitToken(tokens, std::make_shared<TextToken>(currentData));
                    currentData.clear();
                }
                state = State::TagOpen;
            }
            else
            {
                currentData += currentChar;
            }
            break;

        case State::TagOpen:
            if (currentChar == '!')
            {
                if (input.substr(position, 7) == "DOCTYPE")
                {                  // Check for "DOCTYPE"
                    position += 7; // Skip "DOCTYPE"
                    state = State::Doctype;
                }
                else
                {
                    std::cerr << "Unexpected sequence after '<!'" << std::endl;
                    state = State::Data;
                }
            }
            else if (currentChar == '/')
            {
                state = State::EndTagOpen;
            }
            else if (isalpha(currentChar))
            {
                state = State::TagName;
                currentData += currentChar;
                currentToken = std::make_shared<TagToken>(TokenType::StartTag, currentData);
            }
            else
            {
                std::cerr << "Unexpected character after '<': " << currentChar << std::endl;
                state = State::Data;
            }
            break;

        case State::TagName:
            if (isalnum(currentChar))
            {
                currentData += currentChar;
                currentToken->data = currentData;
            }
            else if (isspace(currentChar))
            {
                state = State::AttributeName;
            }
            else if (currentChar == '>')
            {
                emitToken(tokens, currentToken);
                currentToken = nullptr; // Reset after emission
                state = State::Data;
            }
            else if (currentChar == '/')
            {
                currentToken->type = TokenType::SelfClosingTag;
                state = State::AttributeName;
            }
            break;

        case State::AttributeName:
            if (isalnum(currentChar) || currentChar == '-')
            {
                currentAttrName += currentChar;
            }
            else if (currentChar == '=')
            {
                state = State::AttributeValue;
            }
            else if (isspace(currentChar))
            {
                // Treat attributes without values as empty
                if (!currentAttrName.empty())
                {
                    currentToken->addAttribute(currentAttrName, "");
                    currentAttrName.clear();
                }
            }
            else if (currentChar == '>')
            {
                // Finalize attribute without value
                if (!currentAttrName.empty())
                {
                    currentToken->addAttribute(currentAttrName, "");
                    currentAttrName.clear();
                }
                handleAttribute(currentToken, tokens);
                emitToken(tokens, currentToken);
                state = State::Data;
            }
            else
            {
                std::cerr << "Unexpected character in attribute name: " << currentChar << std::endl;
            }
            break;

        case State::AttributeValue:
            if (currentChar == '"' || currentChar == '\'')
            {
                char quote = currentChar;
                while ((currentChar = consumeCharacter()) != quote)
                {
                    currentAttrValue += currentChar;
                }
                currentToken->addAttribute(currentAttrName, currentAttrValue);
                currentAttrName.clear();
                currentAttrValue.clear();
                state = State::AttributeName;
            }
            else if (!isspace(currentChar) && currentChar != '>')
            {
                // Handle unquoted values
                currentAttrValue += currentChar;
                while ((currentChar = consumeCharacter()) != '>' && !isspace(currentChar))
                {
                    currentAttrValue += currentChar;
                }
                currentToken->addAttribute(currentAttrName, currentAttrValue);
                currentAttrName.clear();
                currentAttrValue.clear();
                if (currentChar == '>')
                {
                    emitToken(tokens, currentToken);
                    state = State::Data;
                }
                else
                {
                    state = State::AttributeName;
                }
            }
            else
            {
                std::cerr << "Invalid attribute value syntax!" << std::endl;
                state = State::AttributeName;
            }
            break;

        case State::CommentStart:
            if (currentChar == '-' && input[position] == '-')
            {
                position++; // Consume the second '-'
                state = State::Comment;
            }
            else
            {
                std::cerr << "Unexpected character in comment start" << std::endl;
                state = State::Data;
            }
            break;

        case State::Comment:
            if (currentChar == '-' && input[position] == '-' && input[position + 1] == '>')
            {
                position += 2; // Skip the closing '-->'
                emitToken(tokens, std::make_shared<CommentToken>(currentData));
                currentData.clear();
                state = State::Data;
            }
            else
            {
                currentData += currentChar;
            }
            break;

        case State::Doctype:
            if (isalnum(currentChar))
            {
                currentData += currentChar;
            }
            else if (currentChar == '>')
            {
                emitToken(tokens, std::make_shared<DoctypeToken>(currentData));
                currentData.clear();
                state = State::Data;
            }
            break;

        case State::EndTagOpen:
            if (isalpha(currentChar))
            {
                currentData += currentChar;
                state = State::TagName;
            }
            else
            {
                std::cerr << "Unexpected character in end tag: " << currentChar << std::endl;
                state = State::Data;
            }
            break;
        }
    }

    if (!currentData.empty() && state != State::Data)
    {
        std::cerr << "Unfinished token detected: " << currentData << std::endl;
        if (state == State::TagName)
        {
            emitToken(tokens, std::make_shared<TagToken>(TokenType::StartTag, currentData));
        }
        else if (state == State::Comment)
        {
            emitToken(tokens, std::make_shared<CommentToken>(currentData));
        }
        currentData.clear();
    }

    return tokens;
}

char HTML5Tokenizer::consumeCharacter()
{
    return input[position++];
}

void HTML5Tokenizer::emitToken(std::vector<std::shared_ptr<Token>> &tokens, std::shared_ptr<Token> token)
{
    tokens.push_back(token);
    std::cout << "Emitting token: " << token->toString() << std::endl;
    currentData.clear(); // Clear accumulated data
}

void HTML5Tokenizer::handleAttribute(std::shared_ptr<TagToken> &token, std::vector<std::shared_ptr<Token>> &tokens)
{
    if (!currentAttrName.empty() && !currentAttrValue.empty())
    {
        token->addAttribute(currentAttrName, currentAttrValue);
    }
}
