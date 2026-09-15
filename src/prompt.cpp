#include "prompt.h"

#include <iostream>
#include <sstream>
#include <array>
#include <climits>

#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

std::optional<std::string> Prompt::get_username() const
{
	uid_t uid = getuid();
    struct passwd* pw = getpwuid(uid);

	if (pw == nullptr)
	{
		return std::nullopt;
	}

	return std::string{pw->pw_name};
}

std::optional<std::string> Prompt::get_hostname() const
{
	std::array<char, HOST_NAME_MAX + 1> hostname;

	bool success = gethostname(hostname.data(), hostname.size()) == 0;
    
	return success
		? std::optional{std::string{hostname.data()}}
		: std::nullopt;
}

std::string Prompt::build_prompt() const
{
	std::stringstream ss;

	ss << "[";

	if (auto username = get_username())
	{
		ss << *username;
	}

	if (auto hostname = get_hostname())
	{
		ss << "@";
		ss << *hostname;
	}

	ss << "] > ";
	
	return ss.str();
}

void Prompt::print_prompt() const
{
	std::cout << build_prompt();
}

bool Prompt::read_input()
{
	return static_cast<bool>(std::getline(std::cin, m_buffer));
}

std::optional<std::string_view> Prompt::prompt()
{
	print_prompt();
	bool success = read_input();

	return success
		? std::optional{std::string_view{m_buffer}}
		: std::nullopt;
}
