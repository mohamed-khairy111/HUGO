#ifndef TOKENIZER_H // Start of the header guard to prevent multiple  inclusions of this file
#define TOKENIZER_H

#include <string>        // Provides the std::string class for text handling
#include <vector>        // Provides the std::vector class for dynamic arrays
#include <memory>        // Provides std::shared_ptr for memory management
#include <unordered_map> // Provides std::unordered_map for hash table-based key-value storage

// Enum class to define the different types of tokens the tokenizer will generate
enum class TokenType
{
    StartTag, // Represents an opening tag like <div>
    EndTag,   // Represents a closing tag like </div>
    Text,     // Represents plain text content between tags
    Attribute // Represents an attribute like class="value"
};

// Struct to represent a single token
struct Token
{
    TokenType type;   // Type of the token (StartTag, EndTag, Text, or Attribute)
    std::string data; // the content of the token (e.g., tag name or text)
    std::vector<std::pair<std::string, std::string>> attributes;
    // Stores attributes as key-value pairs (e.g., "id"="123")

    Token() : type(TokenType::Text), data("") {} // Default constructor initializing type to Text and data to an empty string

    Token(TokenType t, const std::string &d) : type(t), data(d) {}
    // Parameterized  constructor initializing type and data with specified values
};

// The main Tokenizer class for parsing HTML input into tokens
class Tokenizer
{
public:
    explicit Tokenizer(const std::string &input); // Constructor to initialize the tokenizer with an input string
    std::vector<std::shared_ptr<Token>> tokenize();
    // The main function that performs tokenization and returns a vector of tokens

private:
    std::string inputBuffer;      // Stores the input HTML string to be tokenized
    size_t currentPosition;       // Tracks the current character position in inputBuffer
    std::string currentTokenData; // Temporary storage for the current token's data (e.g., tag name or text)

    // Enum class defining the various states of the tokenizer's state machine
    enum class State
    {
        Data,           // Reading plain text content
        TagOpen,        // Encountered a '<', determining whether it's a start or end tag
        TagName,        // Reading the name of a tag
        TagClose,       // Reading the name of an end tag (after </)
        SelfClosingTag, // Encountered a '/>' indicating a self-closing tag
        AttributeName,  // Reading an attribute name inside a tag
        AttributeValue  // Reading an attribute value inside a tag
    };

    State state;                       // Current state of the tokenizer's state machine
    std::string currentAttributeName;  // Temporary storage for the name of the current attribute
    std::string currentAttributeValue; // Temporary storage for the value of the current attribute

    // Static map to decode HTML entities (e.g., &lt; -> '<')
    static std::unordered_map<std::string, char> entityMap;

    void switchState(State newState); // Changes the tokenizer's current state
    char consumeCharacter();          // Consumes and returns the next character from the inputBuffer

    // Creates a token with the specified type and data, and adds it to the tokens vector
    void emitToken(std::vector<std::shared_ptr<Token>> &tokens, TokenType type, const std::string &data = "");

    // Decodes HTML entities (e.g., &amp; -> '&') in the provided string
    std::string decodeEntities(const std::string &input);
};

#endif
