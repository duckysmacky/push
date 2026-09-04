#pragma once

#include <string_view>
#include <optional>

struct Token
{
    std::string_view value;
};

class Tokenizer
{
private:
    std::string_view m_raw;
    size_t m_pos;

public:
    Tokenizer(std::string_view raw)
        : m_raw(raw), m_pos(0)
    {}

    std::optional<Token> next_token();
};