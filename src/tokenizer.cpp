#include "tokenizer.h"

#include <iostream>
#include <unordered_map>
#include <cctype>

std::string Token::to_string() const
{
	switch (type)
	{
        case TokenType::Unknown: return "unknown";
        case TokenType::Invalid: return "invalid";
        case TokenType::Empty: return "empty";
		case TokenType::Word: return std::string{*value};
        case TokenType::LeftArrow: return "<";
        case TokenType::DoubleLeftArrow: return "<<";
        case TokenType::RightArrow: return ">";
        case TokenType::DoubleRightArrow: return ">>";
        case TokenType::And: return "&";
        case TokenType::DoubleAnd: return "&&";
        case TokenType::Or: return "|";
        case TokenType::DoubleOr: return "||";
        case TokenType::Semicolon: return ";";
		default: return "unknown";
	}
}

std::ostream& operator<<(std::ostream& os, const Token& token)
{
	return os << "Token(" << token.to_string() << ")";
}

inline std::optional<char> Tokenizer::at(size_t pos) const
{
	return (pos >= m_raw.size())
		? std::nullopt
		: std::optional{ m_raw.at(pos) };
}

inline std::optional<char> Tokenizer::current() const
{
	return at(m_pos);
}

inline std::optional<char> Tokenizer::peek() const
{
	return at(m_pos + 1);
}

inline size_t Tokenizer::advance()
{
	return ++m_pos;
}

std::optional<Token> Tokenizer::peek_token()
{
	if (m_current_token.has_value())
	{
		return m_current_token;
	}

	if (!current().has_value())
	{
		return std::nullopt;
	}

	static const std::unordered_map<char, TokenType> token_chars = {
		{'<', TokenType::LeftArrow},
		{'>', TokenType::RightArrow},
		{'&', TokenType::And},
		{'|', TokenType::Or},
		{';', TokenType::Semicolon},
	};
	static const std::unordered_map<char, TokenType> double_token_chars = {
		{'>', TokenType::DoubleLeftArrow},
		{'>', TokenType::DoubleRightArrow},
		{'&', TokenType::DoubleAnd},
		{'|', TokenType::DoubleOr},
	};

    size_t word_start = -1, word_length = 0;
    bool is_quoted = false;
    char quote_symbol = '\0';

	TokenType token_type = TokenType::Empty;

    while (auto c = current())
    {
		// quotes
        if (c == '\'' || c == '"')
        {
			token_type = TokenType::Word;

            if (!is_quoted)
            {
				// start quotes
				is_quoted = true;
				quote_symbol = *c;
				advance();
				continue;
            }
			else if (c == quote_symbol)
            {
				// end quotes
				// TODO: add Cow string class for when parsing `"text"text`
				advance();
				break;
            }
        }

		// quoted characters
		if (is_quoted)
		{
			if (word_start == -1)
			{
				word_start = m_pos;
			}

			++word_length;
			advance();
			continue;
		}

		// whitespace
        bool is_whitespace = static_cast<bool>(std::isspace(static_cast<unsigned char>(*c)));
        if (is_whitespace)
        {
			if (token_type == TokenType::Word)
			{
				advance();
				break;
			}
			else
			{
				advance();
				continue;
			}
        }
		
		bool is_token_char = token_chars.contains(*c);
		// other characters
		if (!is_token_char)
		{
			token_type = TokenType::Word;

			if (word_start == -1)
			{
				word_start = m_pos;
			}

			++word_length;
			advance();
		}
		// token character but current token is word
		else if (token_type == TokenType::Word)
		{
			break;
		}
		// token character
		else
		{
			token_type = token_chars.at(*c);

			if (auto nc = peek(); nc.has_value() && c == nc && double_token_chars.contains(*nc))
			{
				token_type = double_token_chars.at(*nc);
				advance();
			}

			advance();
			break;
		}
    }

	if (token_type == TokenType::Word)
	{
		if (word_length == 0)
		{
			m_current_token = Token::of_type(TokenType::Empty);
		}
		else
		{
			std::string_view word_value = m_raw.substr(word_start, word_length);
			m_current_token = Token::of_word(word_value);
		}
	}
	else
	{
		m_current_token = Token::of_type(token_type);
	}

	return m_current_token;
}

std::optional<Token> Tokenizer::consume_token()
{
	auto current_token = peek_token();

	m_current_token.reset();

	return current_token;
}

std::vector<Token> Tokenizer::parse_tokens()
{
	std::vector<Token> tokens;

	while (auto token = consume_token())
	{
		tokens.push_back(*token);
	}

	return tokens;
}

