#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>
#include <memory>

enum class TokenType
{
    StartTag,
    EndTag,
    Text,
    Attribute
};

struct Token
{
    TokenType type;
    std::string data;
    std::vector<std::pair<std::string, std::string>> attributes;
};

class Tokenizer
{
public:
    explicit Tokenizer(const std::string &input);
    std::vector<std::shared_ptr<Token>> tokenize();

private:
    std::string inputBuffer;
    size_t currentPosition;
    std::string currentTokenData;

    enum class State
    {
        Data,
        TagOpen,
        TagName,
        TagClose,
        SelfClosingTag,
        AttributeName,
        AttributeValue
    };

    State state;

    void switchState(State newState);
    char consumeCharacter();
    void emitToken(std::vector<std::shared_ptr<Token>> &tokens, TokenType type, const std::string &data = "");
};

#endif