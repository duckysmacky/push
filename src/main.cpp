#include <iostream>
#include <vector>

#include <sys/wait.h>
#include <unistd.h>

#include "prompt.h"
#include "parser.h"
#include "executor.h"

int main(int argc, char* argv[])
{
	Executor executor;
    Prompt prompt;

    while (auto input = prompt.prompt())
    {
        Parser parser(*input);
		std::vector<CommandEntry> commands;

		try
		{
			commands = parser.parse();
		}
		catch (const ParserException& e)
		{
			std::cerr << "parser error: " << e.what() << std::endl;
			continue;
		}

        if (commands.empty())
        {
			continue;
		}

		try
		{
			int exit_code = executor.execute(commands);
			// TODO: prompt.set_exit_code(exit_code)
		}
		catch (const ExecutorException& e)
		{
			std::cerr << "executor error: " << e.what() << std::endl;
			continue;
		}
    }

    return 0;
}

