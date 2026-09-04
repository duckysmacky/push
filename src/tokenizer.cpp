#include "tokenizer.h"

#include <cctype>

std::optional<Token> Tokenizer::next_token()
{
    if (m_pos >= m_raw.size())
        return std::nullopt;

    size_t current_pos = m_pos;
    size_t token_length = 1;
    bool is_quoted = false;
    char quote_symbol = '\0';

    while (current_pos < m_raw.size())
    {
        char c = m_raw.at(current_pos);

        if (c == '\'' || c == '"')
        {
            if (is_quoted && c == quote_symbol)
            {
                is_quoted = false;
                quote_symbol = '\0';
            }
            else if (!is_quoted)
            {
                is_quoted = true;
                quote_symbol = c;
            }
        }        

        bool is_whitespace = std::isspace(static_cast<unsigned char>(c));
        if (is_whitespace && !is_quoted)
        {
            break;
        }

        ++current_pos;
        token_length = current_pos - m_pos;
    }

    std::string_view token_value = m_raw.substr(m_pos, token_length);
    Token token = { token_value };

    m_pos = current_pos + 1;
    return token;
}