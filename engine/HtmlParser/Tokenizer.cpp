#include "Tokenizer.h" // Includes the Tokenizer class and associated declarations from the header file
#include <cctype>      // Provides functions like isalpha(), isalnum(), and isspace() for character checks
#include <iostream>    // Used for error logging with std::cerr
#include <sstream>     // Provides stringstream for text processing if needed in future extenions

// Static map initialization for HTML entity decoding
std::unordered_map<std::string, char> Tokenizer::entityMap = {
    {"amp", '&'},    // &amp; -> &
    {"lt", '<'},     // &lt;  -> <
    {"gt", '>'},     // &gt;  -> >
    {"quot", '"'},   // &quot; -> "
    {"apos", '\''}}; // &apos; -> '

std::string Tokenizer::decodeEntities(const std::string &input)
{
    std::string result; // Stores the decoded output
    size_t i = 0;       // Iterator for traversing the input string

    while (i < input.size()) // Loop through all characters in the input
    {
        if (input[i] == '&') // Check if the current character start an HTML entity
        {
            size_t semicolonPos = input.find(';', i); // Find the position of the ending semicolon
            if (semicolonPos != std::string::npos)    // Ensure the semicolon exists
            {

                std::string entity = input.substr(i + 1, semicolonPos - i - 1); // Extract the entity name (e.g., "lt" from "&lt;")

                // Named entity
                if (entityMap.find(entity) != entityMap.end())
                {
                    result += entityMap[entity]; // Replace with the corresponding character
                }
                // Numeric entity
                else if (entity[0] == '#')
                {
                    try
                    {
                        char decodeChar = static_cast<char>(std::stoi(entity.substr(1))); // Convert numeric entity to a character
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

                i = semicolonPos + 1; // Move past the semicolon
            }
            else
            {
                result += input[i++]; // if no semicolon, treat '&' as a normal character
            }
        }
        else
        {
            result += input[i++]; // Append normal characters directly to the result
        }
    }
    return result; // Return the fully decoded string
}

// Initializes the tokenizer with:
// - inputBuffer: Stores the input HTML string
// - currentPosition: Starts reading from the first character
// - state: Initial state is set to Data, indication plain text reading
Tokenizer::Tokenizer(const std::string &input)
    : inputBuffer(input), currentPosition(0), state(State::Data) {}

std::vector<std::shared_ptr<Token>> Tokenizer::tokenize()
{
    std::vector<std::shared_ptr<Token>> tokens; // Output vector to store generated tokens

    auto currentToken = std::make_shared<Token>(); // Temporary token for accumulating data

    while (currentPosition < inputBuffer.size()) // Loops through all the Html
    {
        char currentChar = consumeCharacter(); // Read the next character

        switch (state) // Handle the current character based on the tokenizer's state
        {
        case State::Data:
            if (currentChar == '<') // A '<' indicates the start of a tag
            {
                if (!currentTokenData.empty())
                {
                    emitToken(tokens, TokenType::Text, decodeEntities(currentTokenData)); // Emit any accumulated text as a Text token
                    currentTokenData.clear();                                             // Clear the temporary storage for the next token
                }
                switchState(State::TagOpen); // Switch to TagOpen state to handle the tag
            }
            else
            {
                currentTokenData += currentChar; // Accumulate plain text
            }
            break;

        case State::TagOpen:
            if (currentChar == '/') // A '/' indicates an end tag
            {
                switchState(State::TagClose);
            }
            else if (isalpha(currentChar)) // if it's a letter, start reading the tag name
            {
                switchState(State::TagName);
                currentTokenData += currentChar;          // Begin accumulating the tag name
                currentToken = std::make_shared<Token>(); // Create a new token
                currentToken->type = TokenType::StartTag; // Set token type to StartTag
                currentToken->data = currentChar;         // Initialize the token data
            }
            else
            {
                std::cerr << "Unexpected character: " << currentChar << std::endl; // Handle invalid characters
            }
            break;

        case State::TagName:
            if (isalnum(currentChar)) // Continue reading the tag name
            {
                currentTokenData += currentChar;
                currentToken->data = currentTokenData;
            }
            else if (currentChar == '>') // End of the tag
            {
                tokens.push_back(currentToken); // Emit the StartTag token
                currentTokenData.clear();
                switchState(State::Data); // Switch back to Data state
            }
            else if (currentChar == '/') // Self-closing tag (e.g., <br/>)
            {
                switchState(State::SelfClosingTag);
            }
            else if (isspace(currentChar)) // Space indicates the start of attributes
            {
                switchState(State::AttributeName);
            }
            else
            {
                std::cerr << "Unexpected character in tag name: " << currentChar << std::endl;
            }
            break;

        case State::AttributeName:
            if (isalnum(currentChar) || currentChar == '-') // Valid attribute name character
            {
                currentAttributeName += currentChar; // Accumulate attribute name
            }
            else if (currentChar == '=') // '=' indicates the start of the attribute value
            {
                switchState(State::AttributeValue);
            }
            else if (isspace(currentChar)) // Allow spaces after attribute names
            {
                // Do nothing, continue to accumulate attributes
            }
            else if (currentChar == '>') // End of the tag
            {
                tokens.push_back(currentToken); // Emit the token
                currentTokenData.clear();
                switchState(State::Data); // Switch back to Data state
            }
            break;

        case State::AttributeValue:
            if (currentChar == '"' || currentChar == '\'') // Quoted attribute value
            {
                char quote = currentChar;                           // Remember the quote type
                currentAttributeValue.clear();                      // Clear the value storage
                while ((currentChar = consumeCharacter()) != quote) // Read until the closing quote
                {
                    currentAttributeValue += currentChar; // Accumulate attribute value
                }
                currentToken->attributes.emplace_back(currentAttributeName, decodeEntities(currentAttributeValue)); // Add the attribute (name-value pair) to the token
                currentAttributeName.clear();                                                                       // Clear attribute name storage
                switchState(State::AttributeName);                                                                  // Switch back to AttributeName state
            }
            else
            {
                std::cerr << "Expected quotes for attribute value!" << std::endl; // Handle missing quotes
            }
            break;

        case State::TagClose:
            if (isalnum(currentChar)) // Accumulate the end tag name
            {
                currentTokenData += currentChar;
            }
            else if (currentChar == '>') // End of the tag
            {
                emitToken(tokens, TokenType::EndTag, currentTokenData); // Emit EndTag token
                currentTokenData.clear();
                switchState(State::Data);
            }
            else
            {
                std::cerr << "Unexpected character in closing tag: " << currentChar << std::endl;
            }
            break;

        case State::SelfClosingTag:
            if (currentChar == '>') // End of a self-closing tag
            {

                tokens.push_back(currentToken); // Emit the token
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

    return tokens; // Return the list of generated tokens
}

void Tokenizer::switchState(State newState)
{
    state = newState; // Update the current state of the tokenizer
}

char Tokenizer::consumeCharacter()
{
    return inputBuffer[currentPosition++]; // Read and advance the position in the input string
}

void Tokenizer::emitToken(std::vector<std::shared_ptr<Token>> &tokens, TokenType type, const std::string &data)
{
    auto token = std::make_shared<Token>(); // Create a new token
    token->type = type;                     // Set the token type
    token->data = data;                     // Set the token data
    tokens.push_back(token);                // Add the token to the tokens vector
}
