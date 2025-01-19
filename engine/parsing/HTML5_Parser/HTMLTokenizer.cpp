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

void HTMLTokenizer::switchState(State newState)
{
    state = newState;
}