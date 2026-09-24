#include "executor.h"

#include <optional>
#include <sstream>
#include <cstring>
#include <cerrno>
#include <iostream>

#include <sys/wait.h>

#include "child.h"

ExitStatus Executor::join_child(pid_t child_pid) const
{
    int status;
    waitpid(child_pid, &status, 0);

    if (WIFEXITED(status))
    {
        return {
			.code = WEXITSTATUS(status),
			.reason = ExitType::Exited,
		};
    }
    else if (WIFSIGNALED(status))
    {
        return {
			.code = WTERMSIG(status),
			.reason = ExitType::Signaled,
		};
    }

	return {
		.code = 0,
		.reason = ExitType::Unknown,
	};
}

void Executor::handle_child(const Command& cmd) const
{
	ChildProcess child(cmd.argv);

	int error = 0;
	child.spawn(error);

	std::stringstream msg;
	switch (error)
	{
		case 0: return;
		case ENOENT: msg << "command not found: " << cmd.argv[0];
		case EACCES: msg << "permission denied: " << cmd.argv[0];
		default: msg << "execution failed: " << std::strerror(error);
	}

	throw ExecutorException(msg.str());
}

int Executor::spawn_command(const Command& cmd)
{
    pid_t pid = fork();

    if (pid < 0)
    {
		throw ExecutorException("fork failed");
    }
    else if (pid == 0) // child process
    {
		handle_child(cmd);
		// never returns
		return 0;
    }
    else // parent process
    {
        ExitStatus status = join_child(pid);
		return status.code;
    }
}

int Executor::execute_command(const Command& cmd, int last_exit_code)
{
	// TODO: add support for "exit"
	return spawn_command(cmd);
}

int Executor::execute_pipeline(const Pipeline& pipeline, bool background)
{
	int last_exit_code = 0;

	// TODO: add background pipeline process support
	if (background)
	{
		throw ExecutorException("background pipeline execution is not supported yet");
	}

	for (const auto& cmd : pipeline.commands)
	{
		last_exit_code = execute_command(cmd, last_exit_code);
	}

	return last_exit_code;
}

int Executor::execute(const std::vector<CommandEntry>& commands)
{
	int last_exit_code = 0;
	std::optional<Separator> last_separator;

	for (const auto& cmd : commands)
	{
		bool background = false;

		if (last_separator)
		{
			if (last_exit_code != 0 && *last_separator == Separator::Sequence)
			{
				if (cmd.separator)
				{
					last_separator = *cmd.separator;
				}

				continue;
			}

			if (*last_separator == Separator::Background)
			{
				background = true;
			}
		}

		last_exit_code = execute_pipeline(cmd.pipeline, background);

		if (cmd.separator)
		{
			last_separator = *cmd.separator;
		}
	}

	return last_exit_code;
};

