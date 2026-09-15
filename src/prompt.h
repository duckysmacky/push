#pragma once

#include <string>
#include <string_view>
#include <optional>

class Prompt
{
	std::string m_buffer;

	void print_prompt() const;

public:
	Prompt() = default;

	std::optional<std::string_view> prompt();
};
