#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

// Enum for token types
enum class TokenType
{
    Doctype,
    StartTag,
    EndTag,
    SelfClosingTag,
    Comment,
    Text
};

// Base class for tokens
class Token
{
public:
    TokenType type;
    std::string data; // Main content (e.g., tag name or text content)

    // Constructor
    Token(TokenType t, const std::string &d) : type(t), data(d) {}

    // Virtual destructor for polymorphism
    virtual ~Token() = default;

    // Optional: Debugging representation
    virtual std::string toString() const = 0;
};

// Token for start and self-closing tags (with attributes)
class TagToken : public Token
{
public:
    std::unordered_map<std::string, std::string> attributes; // Key-value pairs of attributes

    TagToken(TokenType t, const std::string &d) : Token(t, d) {}

    // Add attribute
    void addAttribute(const std::string &name, const std::string &value)
    {
        attributes[name] = value;
    }

    // Debugging representation
    std::string toString() const override
    {
        std::string result = (type == TokenType::StartTag) ? "<" : "<self-closing ";
        result += data;
        for (const auto &[key, value] : attributes)
        {
            result += " " + key + "=\"" + value + "\"";
        }
        result += (type == TokenType::StartTag) ? ">" : "/>";
        return result;
    }
};

// Token for comments
class CommentToken : public Token
{
public:
    CommentToken(const std::string &d) : Token(TokenType::Comment, d) {}

    // Debugging representation
    std::string toString() const override
    {
        return "<!-- " + data + " -->";
    }
};

// Token for text
class TextToken : public Token
{
public:
    TextToken(const std::string &d) : Token(TokenType::Text, d) {}

    // Debugging representation
    std::string toString() const override
    {
        return data;
    }
};

// Token for Doctype
class DoctypeToken : public Token
{
public:
    DoctypeToken(const std::string &d) : Token(TokenType::Doctype, d) {}

    // Debugging representation
    std::string toString() const override
    {
        return "<!DOCTYPE " + data + ">";
    }
};

#endif // TOKEN_H
