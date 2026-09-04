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
    std::string_view m_input;

public:
    Parser(std::string_view input)
        : m_input(input)
    {}

    Command next_command();
};