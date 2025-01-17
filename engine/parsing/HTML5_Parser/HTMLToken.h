#pragma once

#include <string>
#include <vector>

using namespace std;

class Attribute
{
public:
    Attribute(const string &name, const string &value) : attribute_name(name), attribute_value(value) {}

    const string &name() const { return attribute_name; }
    const string &value() const { return attribute_value; }

    void set_value(const string &value) { attribute_value = value; }

private:
    string attribute_name;
    string attribute_value;
};

class Token
{
public:
    enum class TokenType
    {
        DOCTYPE,
        StartTag,
        EndTag,
        Comment,
        Character,
        EndOfFile,
    };

    TokenType type() const { return tokentype; }

private:
    TokenType tokentype;

    struct Doctype_tag
    {
        string name;
        string public_identifier;
        string system_public_identifier;
        bool force_quirks = false;
    };

    struct Tag
    {
        string tag_name;
        bool self_closing = false;
        vector<Attribute> attributes;
    };

    struct comment_or_character
    {
        string data;
    };
};