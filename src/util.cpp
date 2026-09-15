#include "util.h"

std::optional<std::string> util::get_env(std::string_view key)
{
	const char* value = std::getenv(key.data());
	
	return value == nullptr
		? std::nullopt
		: std::optional{std::string{value}};
}
