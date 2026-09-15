#pragma once

#include <string>
#include <string_view>
#include <optional>

class Prompt
{
	std::string m_buffer;

	std::optional<std::string> get_username() const;
	std::optional<std::string> get_hostname() const;
	std::string build_prompt() const;
	void print_prompt() const;
	bool read_input();

public:
	Prompt() = default;

	std::optional<std::string_view> prompt();
};
