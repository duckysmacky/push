#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "tokenizer.h"

struct Command
{
    std::vector<std::string> args;
};

class Parser
{
private:
    Tokenizer m_tokenizer;

public:
    Parser(std::string_view input)
        : m_tokenizer(input)
    {}

    Command next_command();
};
