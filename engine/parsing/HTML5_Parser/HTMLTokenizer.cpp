#include "HTMLTokenizer.h"
#include "HTMLToken.h"
#include <optional>

HTMLTokenizer::HTMLTokenizer(const string &html)
    : inputBuffer(html),
      currentPosition(0),
      state(State::Data),
      return_state(State::Data) {}

void HTMLTokenizer::tokenize()
{
    optional<char> currentChar = next_character();

    switch (state)
    {
    case State::Data:
        if (currentChar.has_value() && currentChar.value() == '&')
        {
            return_state = State::Data;
            switchState(State::CharacterReference);
        }

        if (currentChar.has_value() && currentChar.value() == '>')
        {
            switchState(State::TagOpen);
        }
        break;

    case State::TagOpen:
        if (currentChar.has_value() && currentChar.value() == '!')
        {
            switchState(State::MarkupDeclarationOpen);
        }
        break;

    case State::MarkupDeclarationOpen:
        if (next_few_character_are("DOCTYPE"))
        {
            consume("DOCTYPE");
            switchState(State::DOCTYPE);
        }
        break;

    case State::DOCTYPE:
        if (currentChar.has_value() && (currentChar.value() == '\t' || currentChar.value() == '\a' || currentChar.value() == '\f' || currentChar.value() == ' '))
        {
            switchState(State::BeforeDOCTYPEName);
        }

        break;

    case State::BeforeDOCTYPEName:

        break;

    case State::CharacterReference:
        break;
    }
}

char HTMLTokenizer::next_character()
{
    if (currentPosition >= inputBuffer.length())
    {
        return 0;
    }

    return inputBuffer[currentPosition++];
}
char HTMLTokenizer::peek_character(int offset) const
{
    if (currentPosition + offset >= inputBuffer.length())
    {
        return 0;
    }

    return inputBuffer[currentPosition + offset];
}
void HTMLTokenizer::consume(const string s)
{
    currentPosition += s.length();
}

void HTMLTokenizer::switchState(State newState)
{
    state = newState;

    optional<char> currentChar = next_character();
}

bool HTMLTokenizer::next_few_character_are(const string s) const
{
    for (int i = 0; i < s.length(); i++)
    {
        optional<char> character = peek_character(i);
        if (!character.has_value())
            return false;
        if (character.value() != s[i])
            return false;
    }
    return true;
}