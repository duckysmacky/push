#include "prompt.h"

#include <iostream>

std::optional<std::string_view> Prompt::prompt()
{
	bool success = static_cast<bool>(std::getline(std::cin, m_buffer));

	return success
		? std::optional{std::string_view{m_buffer}}
		: std::nullopt;
}
