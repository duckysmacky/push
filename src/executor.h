#pragma once

#include <unistd.h>

#include "parser.h"

class ExecutorException : public std::runtime_error
{
public:
    explicit ExecutorException(const std::string& msg) 
        : std::runtime_error(msg) {}
};

enum class ExitType
{
	Unknown, Exited, Signaled
};

struct ExitStatus
{
	int code;
	ExitType reason;
};

class Executor
{
	std::vector<CommandEntry> m_commands;

	ExitStatus join_child(pid_t child_pid) const;
	void handle_child(const Command& cmd) const;

	int spawn_command(const Command& cmd);
	int execute_command(const Command& cmd, int last_exit_code);
	int execute_pipeline(const Pipeline& commads, bool background);

public:
	Executor() = default;

	int execute(const std::vector<CommandEntry>& commands);
};
