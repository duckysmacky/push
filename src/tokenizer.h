#pragma once

#include <string_view>
#include <optional>
#include <vector>

// < > >> & && | || ;
enum class TokenType
{
	Unknown, Invalid, Empty,
	Word, 
	LeftArrow, DoubleLeftArrow,
	RightArrow, DoubleRightArrow,
	And, DoubleAnd,
	Or, DoubleOr,
	Semicolon
};

struct Token
{
	TokenType type;
	std::optional<std::string_view> value;

	static Token of_type(TokenType type)
	{
		return Token{
			.type = type,
			.value = std::nullopt,
		};
	}

	static Token of_word(std::string_view value)
	{
		return Token{
			.type = TokenType::Word,
			.value = value
		};
	}

	std::string to_string() const;

	friend std::ostream& operator<<(std::ostream& os, const Token& token);
};

class Tokenizer
{
private:
    std::string_view m_raw;
    size_t m_pos = 0;
	
	inline std::optional<char> at(size_t pos) const;
	inline std::optional<char> current() const;
	inline std::optional<char> peek() const;
	inline size_t advance();

public:
    Tokenizer(std::string_view raw)
        : m_raw(raw)
    {}

    std::optional<Token> next_token();
	std::vector<Token> parse_tokens();
};
