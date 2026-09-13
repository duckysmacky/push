#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include <sys/wait.h>
#include <unistd.h>

#include "parser.h"

namespace fs = std::filesystem;

void handle_child(Command cmd)
{
    fs::path executable(cmd.args[0]);
    std::vector<char*> argv_vec;
    argv_vec.reserve(cmd.args.size() + 1);

    for (std::string& arg : cmd.args)
    {
        argv_vec.push_back(const_cast<char*>(arg.c_str()));
    }
    argv_vec.push_back(nullptr);

    char* const* argv = argv_vec.data();

    execvpe(argv[0], argv, environ);
    // anything after should be unreachable if success

    int err = errno;
    if (err == ENOENT)
    {
        std::cerr << "command not found: " << executable << "\n";
    }
    else if (err == EACCES)
    {
        std::cerr << "permission denied: " << executable << "\n";
    }
    else
    {
        std::cerr << "execution failed: error code " << err << "\n";
    }

    std::exit(err == ENOENT ? 127 : 126); 
}

int handle_parent(pid_t child_pid)
{
    int status;
    waitpid(child_pid, &status, 0);

    if (WIFEXITED(status))
    {
        int exit_code = WEXITSTATUS(status);
        return exit_code;
    }
    else if (WIFSIGNALED(status))
    {
        int signal_code = WTERMSIG(status);
        return signal_code;
    }

    return 0;
}

int handle_command(Command cmd)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        std::cerr << "fork failed" << std::endl;
        return 1;
    }
    else if (pid == 0)
    {
        // child process
        handle_child(std::move(cmd));
        return 0;
    }
    else
    {
        // parent process
        return handle_parent(pid);
    }
}

int main(int argc, char* argv[])
{
    std::string input;

    while (std::getline(std::cin, input))
    {
        Parser parser(input);
        Command cmd = parser.next_command();

        if (cmd.args.empty())
            continue;
        
        if (cmd.args[0] == "exit")
            break;

        int exit_code = handle_command(std::move(cmd));
    }

    return 0;
}

