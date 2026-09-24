#pragma once

#include <vector>
#include <string>

class ChildProcess
{
    std::vector<char*> m_argv_vec;

public:
	ChildProcess(char** argv, size_t size)
		: m_argv_vec{argv, argv + size} {}
	ChildProcess(std::vector<char*> argv_vec)
		: m_argv_vec(std::move(argv_vec)) {}
	ChildProcess(const std::vector<std::string>& args);
	
	void spawn(int& error_code);
};
