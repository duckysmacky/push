#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace util
{

	std::optional<std::string> get_env(std::string_view key);
	
}
