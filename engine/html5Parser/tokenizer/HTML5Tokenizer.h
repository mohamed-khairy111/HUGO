#ifndef HTML5TOKENIZER_H
#define HTML5TOKENIZER_H

#include "Token.h"
#include <string>
#include <vector>
#include <memory>

class HTML5Tokenizer
{
public:
    explicit HTML5Tokenizer(const std::string &html);
    std::vector<std::shared_ptr<Token>> tokenize();

private:
    std::string input;            // HTML input
    size_t position;              // Current position in the input
    std::string currentData;      // Accumulator for current text
    std::string currentAttrName;  // Current attribute name
    std::string currentAttrValue; // Current attribute value

    enum class State
    {
        Data,           // Default state, handling text
        TagOpen,        // Handling '<'
        TagName,        // Handling tag names
        EndTagOpen,     // Handling '</'
        AttributeName,  // Parsing attribute names
        AttributeValue, // Parsing attribute values
        CommentStart,   // Handling `<!--`
        Comment,        // Parsing comment content
        Doctype         // Parsing doctype
    };

    State state; // Current state of the tokenizer

    // Utility methods
    char consumeCharacter();
    void emitToken(std::vector<std::shared_ptr<Token>> &tokens, std::shared_ptr<Token> token);
    void handleAttribute(std::shared_ptr<TagToken> &token, std::vector<std::shared_ptr<Token>> &tokens);
};

#endif // HTML5TOKENIZER_H
