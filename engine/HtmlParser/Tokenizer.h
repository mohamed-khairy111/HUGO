#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

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

    Token() : type(TokenType::Text), data("") {}
    Token(TokenType t, const std::string &d) : type(t), data(d) {}
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
    std::string currentAttributeName;
    std::string currentAttributeValue;

    static std::unordered_map<std::string, char> entityMap;
    void switchState(State newState);
    char consumeCharacter();
    void emitToken(std::vector<std::shared_ptr<Token>> &tokens, TokenType type, const std::string &data = "");
    std::string decodeEntities(const std::string &input);
};

#endif
